/*
 * MTV variable-server path tests.
 *
 * These tests verify two classes of bug fixes:
 *
 * 1. Issue #2020 / PR #2092: MTV used "cond[N]" and "act[N]" as variable
 *    server paths instead of the correct "condition_list[N][0]" and
 *    "action_list[N][0]" field names.
 *
 * 2. Issue #2139 / PR #2141: events with index N>=1 failed to load.
 *    MemoryManager_ref_dim.cpp double-dereferenced the address in its
 *    dynamic-array bounds check, so "mtv_list[1]" (or any index >= 1 on a
 *    named TMM pointer array) was rejected as out of bounds and every path
 *    under it resolved to BAD_REF. The original tests missed this because
 *    they only asserted on event index 0 values, and a bad reference is
 *    still added and sent (as the string "BAD_REF"), so token/size counts
 *    look normal. Lesson: assert on *values* at index >= 1, and scan for
 *    BAD_REF; counting tokens proves nothing.
 *
 * The variable paths used here must stay in sync with the paths MtvApp.java
 * builds (see "get cyclical event data" in MtvApp.java and
 * Trick::MTV::send_event_data() in MTV.cpp). #2139 happened partly because
 * these tests hand-wrote correct paths while MtvApp.java emitted broken
 * ones — if you change the paths in either place, change them here too.
 *
 * Protocol note (ASCII mode):
 *   var_send_once("x")  ->  "5\t<value>\n"
 *   var_send() data     ->  "0\t<v1>\t<v2>...\n"
 *   var_send_list_size  ->  "3\t<count>\n"    (from mtv_send_event_data)
 */

#include "test_client.hh"

#include <sstream>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Split a tab-delimited line into tokens; strips the trailing newline.
static std::vector<std::string> split_tabs(const std::string& line)
{
    std::vector<std::string> tokens;
    std::istringstream ss(line);
    std::string tok;
    while (std::getline(ss, tok, '\t'))
    {
        // strip trailing newline from the last token
        if (!tok.empty() && tok.back() == '\n')
            tok.pop_back();
        tokens.push_back(tok);
    }
    return tokens;
}

// Request one variable value in ASCII mode; returns the value string.
// Response format: "5\t<value>\n"
static std::string get_once(Socket& s, const std::string& varpath)
{
    s << "trick.var_send_once(\"" + varpath + "\")\n";
    std::string reply = s.recv_line();
    auto tokens       = split_tabs(reply);
    if (tokens.size() < 2)
        return "";
    return tokens[1];
}

// Build the var_add commands MtvApp.java sends for one event's cyclic status
// variables, and bump var_count by the number of variables added.
// MUST stay in sync with the "get cyclical event data" section of
// MtvApp.java — replicating MtvApp's real subscription set is the point.
static std::string mtvapp_event_vars(int ii, int cond_count, int act_count, int& var_count)
{
    std::ostringstream cmd;
    std::string event = "trick_ip.mtv.mtv_list[" + std::to_string(ii) + "][0]";

    const char* event_fields[]
        = { "active", "fired_time", "fired_count", "ran_time", "ran_count", "manual", "manual_fired", "hold", "added" };
    for (const char* field : event_fields)
    {
        cmd << "trick.var_add(\"" << event << "." << field << "\"); ";
        var_count++;
    }
    const char* cond_fields[] = { "enabled", "fired_time", "fired_count", "hold" };
    for (int jj = 0; jj < cond_count; jj++)
    {
        for (const char* field : cond_fields)
        {
            cmd << "trick.var_add(\"" << event << ".condition_list[" << jj << "][0]." << field << "\"); ";
            var_count++;
        }
    }
    const char* act_fields[] = { "enabled", "ran_time", "ran_count" };
    for (int jj = 0; jj < act_count; jj++)
    {
        for (const char* field : act_fields)
        {
            cmd << "trick.var_add(\"" << event << ".action_list[" << jj << "][0]." << field << "\"); ";
            var_count++;
        }
    }
    return cmd.str();
}

// Fail the calling test if any token in a var server reply is BAD_REF.
// An unresolvable path is still added and sent as the literal string
// "BAD_REF", so counting tokens can never detect a broken path.
static void expect_no_bad_ref(const std::vector<std::string>& tokens, const std::string& context)
{
    for (size_t i = 0; i < tokens.size(); i++)
    {
        EXPECT_NE(tokens[i], "BAD_REF") << context << ": token " << i
                                        << " is BAD_REF — a variable path did not resolve";
    }
}

// ---------------------------------------------------------------------------
// Test 1 – EventCount
//
// Verify trick_ip.mtv.mtv_count equals the number of events we registered
// (event_a and event_b -> 2).
// ---------------------------------------------------------------------------
TEST_F(MtvTest, EventCount)
{
    ASSERT_EQ(status, 0) << "Could not connect to variable server";

    std::string val = get_once(socket, "trick_ip.mtv.mtv_count");
    EXPECT_EQ(val, "2") << "Expected 2 events in mtv_count, got: " << val;
}

// ---------------------------------------------------------------------------
// Test 2 – SendEventDataComments
//
// Call trick.mtv_send_event_data() and verify the full data block for BOTH
// events. Asserting on event_b (mtv_list index 1) is what catches the #2139
// MemoryManager bounds-check regression: pre-fix, every mtv_list[1][0].x
// path resolved to BAD_REF while the variable count stayed correct.
//
// For event_a (1 condition, 1 action, before_after=0):
//   base fields: name, active, added, condition_count, action_count,
//                before_after   = 6
//   + cond comments:              + 1
//   + act comments:               + 1
//   subtotal event_a:             = 8
//
// For event_b (2 conditions, 2 actions, before_after=0):
//   base fields:                  = 6
//   + cond comments:              + 2
//   + act comments:               + 2
//   subtotal event_b:             = 10
//
// Total variables:                = 18
// ---------------------------------------------------------------------------
TEST_F(MtvTest, SendEventDataComments)
{
    ASSERT_EQ(status, 0) << "Could not connect to variable server";

    // Trigger the server-side batch send.
    socket << "trick.mtv_send_event_data()\n";

    // First reply: list-size header "3\t<count>\n"
    std::string size_line = socket.recv_line();
    auto size_tokens      = split_tabs(size_line);
    ASSERT_GE(size_tokens.size(), 2u) << "Malformed list-size reply: " << size_line;
    EXPECT_EQ(size_tokens[0], "3") << "Expected VS_LIST_SIZE message type (3)";

    int var_count = std::stoi(size_tokens[1]);
    EXPECT_EQ(var_count, 18) << "Expected 18 variables for 2 events (8 + 10)";

    // Second reply: the data "0\t<v0>\t<v1>...\n"
    std::string data_line = socket.recv_line();
    auto data             = split_tabs(data_line);
    ASSERT_GE(data.size(), 19u) // message-type token + 18 values
        << "Not enough tokens in data reply. Got: " << data_line;

    expect_no_bad_ref(data, "mtv_send_event_data reply");

    // data[0] = message type "0"
    //
    // event_a block (mtv_list[0][0]):
    // data[1] = name, data[2] = active, data[3] = added,
    // data[4] = condition_count, data[5] = action_count,
    // data[6] = before_after,
    // data[7] = condition_list[0][0].comment  <- broken pre-#2092
    // data[8] = action_list[0][0].comment     <- broken pre-#2092
    EXPECT_EQ(data[1], "event_a");
    EXPECT_EQ(data[4], "1") << "event_a should have 1 condition";
    EXPECT_EQ(data[5], "1") << "event_a should have 1 action";
    EXPECT_EQ(data[7], "cond_comment_0")
        << "condition_list[0][0].comment mismatch – was the cond->condition_list[N][0] fix applied?";
    EXPECT_EQ(data[8], "act_comment_0")
        << "action_list[0][0].comment mismatch – was the act->action_list[N][0] fix applied?";

    // event_b block (mtv_list[1][0]) — every value here was BAD_REF pre-#2141:
    // data[9]  = name, data[10] = active, data[11] = added,
    // data[12] = condition_count, data[13] = action_count,
    // data[14] = before_after,
    // data[15] = condition_list[0][0].comment (condition_var: defaults to var name)
    // data[16] = condition_list[1][0].comment
    // data[17] = action_list[0][0].comment
    // data[18] = action_list[1][0].comment
    EXPECT_EQ(data[9], "event_b") << "mtv_list[1][0].name failed to resolve – #2139 regression?";
    EXPECT_EQ(data[12], "2") << "event_b should have 2 conditions";
    EXPECT_EQ(data[13], "2") << "event_b should have 2 actions";
    EXPECT_EQ(data[15], "mobj.fire_condition");
    EXPECT_EQ(data[16], "cond_b_comment_1") << "condition_list[1][0].comment failed – condition index >= 1 broken?";
    EXPECT_EQ(data[17], "act_b_comment_0");
    EXPECT_EQ(data[18], "act_b_comment_1") << "action_list[1][0].comment failed – action index >= 1 broken?";
}

// ---------------------------------------------------------------------------
// Test 3 – CyclicConditionList
//
// Subscribe cyclically to condition_list fields at index 0 (event_a) AND at
// event index 1 / condition index 1 (event_b's second condition). The
// index >= 1 paths are the #2139 regression case: pre-fix they resolved to
// BAD_REF, while index 0 worked.
// ---------------------------------------------------------------------------
TEST_F(MtvTest, CyclicConditionList)
{
    ASSERT_EQ(status, 0) << "Could not connect to variable server";

    // Subscribe to all four condition_list fields MtvApp.java uses.
    socket << "trick.var_add(\"trick_ip.mtv.mtv_list[0][0].condition_list[0][0].enabled\")\n"
              "trick.var_add(\"trick_ip.mtv.mtv_list[0][0].condition_list[0][0].fired_time\")\n"
              "trick.var_add(\"trick_ip.mtv.mtv_list[0][0].condition_list[0][0].fired_count\")\n"
              "trick.var_add(\"trick_ip.mtv.mtv_list[0][0].condition_list[0][0].hold\")\n"
              "trick.var_add(\"trick_ip.mtv.mtv_list[1][0].condition_list[1][0].enabled\")\n"
              "trick.var_add(\"trick_ip.mtv.mtv_list[1][0].condition_list[1][0].fired_time\")\n"
              "trick.var_add(\"trick_ip.mtv.mtv_list[1][0].condition_list[1][0].fired_count\")\n"
              "trick.var_add(\"trick_ip.mtv.mtv_list[1][0].condition_list[1][0].hold\")\n"
              "trick.var_cycle(0.1)\n";

    // Read one cycle of data; the var server should send back 8 values.
    std::string reply = socket.recv_line();
    auto tokens       = split_tabs(reply);

    // Message type 0 + 8 value tokens = 9 tokens minimum.
    ASSERT_GE(tokens.size(), 9u) << "condition_list subscription returned too few tokens.\n"
                                 << "Reply: " << reply;

    EXPECT_EQ(tokens[0], "0") << "Expected var_send data message type (0)";
    expect_no_bad_ref(tokens, "condition_list cyclic reply");

    // enabled defaults to true (1) for a freshly created condition.
    EXPECT_EQ(tokens[1], "1") << "condition_list[0][0].enabled should be 1 (true)";
    EXPECT_EQ(tokens[5], "1") << "mtv_list[1][0].condition_list[1][0].enabled should be 1 – index >= 1 broken (#2139)?";

    socket << "trick.var_clear()\n";
}

// ---------------------------------------------------------------------------
// Test 4 – CyclicActionList
//
// Same as Test 3 but for action_list fields.
// ---------------------------------------------------------------------------
TEST_F(MtvTest, CyclicActionList)
{
    ASSERT_EQ(status, 0) << "Could not connect to variable server";

    socket << "trick.var_add(\"trick_ip.mtv.mtv_list[0][0].action_list[0][0].enabled\")\n"
              "trick.var_add(\"trick_ip.mtv.mtv_list[0][0].action_list[0][0].ran_time\")\n"
              "trick.var_add(\"trick_ip.mtv.mtv_list[0][0].action_list[0][0].ran_count\")\n"
              "trick.var_add(\"trick_ip.mtv.mtv_list[1][0].action_list[1][0].enabled\")\n"
              "trick.var_add(\"trick_ip.mtv.mtv_list[1][0].action_list[1][0].ran_time\")\n"
              "trick.var_add(\"trick_ip.mtv.mtv_list[1][0].action_list[1][0].ran_count\")\n"
              "trick.var_cycle(0.1)\n";

    std::string reply = socket.recv_line();
    auto tokens       = split_tabs(reply);

    // Message type 0 + 6 value tokens = 7 tokens minimum.
    ASSERT_GE(tokens.size(), 7u) << "action_list subscription returned too few tokens.\n"
                                 << "Reply: " << reply;

    EXPECT_EQ(tokens[0], "0") << "Expected var_send data message type (0)";
    expect_no_bad_ref(tokens, "action_list cyclic reply");

    EXPECT_EQ(tokens[1], "1") << "action_list[0][0].enabled should be 1 (true)";
    EXPECT_EQ(tokens[4], "1") << "mtv_list[1][0].action_list[1][0].enabled should be 1 – index >= 1 broken (#2139)?";

    socket << "trick.var_clear()\n";
}

// ---------------------------------------------------------------------------
// Test 5 – MtvAppCyclicVarSet
//
// Replicate the exact cyclic subscription MtvApp.java builds after loading
// the event list (time vars + per-event status vars for both events), then
// verify every variable resolves to a real value. This is what MTV actually
// does when it connects, so a path MtvApp constructs that the variable
// server cannot resolve fails here instead of only failing when a user
// connects the real GUI.
// ---------------------------------------------------------------------------
TEST_F(MtvTest, MtvAppCyclicVarSet)
{
    ASSERT_EQ(status, 0) << "Could not connect to variable server";

    int var_count    = 0;
    std::string cmd  = "trick.var_add(\"trick_sys.sched.time_tics\"); "
                       "trick.var_add(\"trick_ip.mtv.mtv_update_ticker\"); ";
    var_count       += 2;
    cmd             += mtvapp_event_vars(0, 1, 1, var_count); // event_a: 1 condition, 1 action
    cmd             += mtvapp_event_vars(1, 2, 2, var_count); // event_b: 2 conditions, 2 actions
    socket << cmd + "\n" + "trick.var_cycle(0.1)\n";

    std::string reply = socket.recv_line();
    auto tokens       = split_tabs(reply);

    ASSERT_EQ(tokens.size(), (size_t)var_count + 1) // message-type token + values
        << "MtvApp cyclic subscription returned wrong token count.\n"
        << "Reply: " << reply;

    EXPECT_EQ(tokens[0], "0") << "Expected var_send data message type (0)";
    expect_no_bad_ref(tokens, "MtvApp cyclic reply");

    socket << "trick.var_clear()\n";
}

// ---------------------------------------------------------------------------
// Test 6 – UpdateTicker
//
// Verify mtv_update_ticker increments when an event is deleted.
// This confirms the MTV bookkeeping that drives MtvApp's re-sync logic.
// ---------------------------------------------------------------------------
TEST_F(MtvTest, UpdateTicker)
{
    ASSERT_EQ(status, 0) << "Could not connect to variable server";

    // Read baseline ticker.
    std::string before = get_once(socket, "trick_ip.mtv.mtv_update_ticker");
    int ticker_before  = std::stoi(before);

    // Delete event_b through the variable server (Python executed in sim).
    socket << "trick.delete_event(event_b)\n";

    // Give the sim a moment to process the deletion.
    usleep(300000); // 300 ms

    std::string after = get_once(socket, "trick_ip.mtv.mtv_update_ticker");
    int ticker_after  = std::stoi(after);

    EXPECT_GT(ticker_after, ticker_before) << "mtv_update_ticker should have incremented after delete_event()";
}

// ---------------------------------------------------------------------------
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();

    // Signal pass/fail back to the sim so it exits with the right return code.
    // Use a fresh socket so the per-test fixtures' var_exit calls don't interfere.
    Socket ctrl;
    if (ctrl.init("localhost", MTV_TEST_PORT) == 0)
    {
        if (result == 0)
        {
            ctrl << "mobj.success()\n";
        }
        else
        {
            ctrl << "mobj.fail()\n";
        }
        ctrl << "trick.stop()\n";
        ctrl.close();
    }

    return result;
}
