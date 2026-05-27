/*
 * MTV variable-server path tests.
 *
 * These tests verify the bug fix where MTV used "cond[N]" and "act[N]" as
 * variable server paths instead of the correct "condition_list[N]" and
 * "action_list[N]" field names.
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
// Call trick.mtv_send_event_data() and verify:
//   a) The list-size message arrives with the right variable count.
//   b) condition_list[0].comment == "cond_comment_0"   (was broken pre-fix)
//   c) action_list[0].comment    == "act_comment_0"    (was broken pre-fix)
//
// For event_a (1 condition, 1 action, before_after=0):
//   base fields: name, active, added, condition_count, action_count,
//                before_after   = 6
//   + cond comments:              + 1
//   + act comments:               + 1
//   subtotal event_a:             = 8
//
// For event_b (1 condition, 1 action, before_after=0):
//   same layout:                  = 8
//
// Total variables:                = 16
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
    EXPECT_EQ(var_count, 16) << "Expected 16 variables for 2 events (8 each)";

    // Second reply: the data "0\t<v0>\t<v1>...\n"
    std::string data_line = socket.recv_line();
    auto data             = split_tabs(data_line);
    ASSERT_GE(data.size(), 17u) // message-type token + 16 values
        << "Not enough tokens in data reply. Got: " << data_line;

    // data[0] = message type "0"
    // data[1] = event_a.name
    // data[2] = event_a.active
    // data[3] = event_a.added
    // data[4] = event_a.condition_count
    // data[5] = event_a.action_count
    // data[6] = event_a.before_after
    // data[7] = condition_list[0][0].comment  <- was broken pre-fix
    // data[8] = action_list[0][0].comment     <- was broken pre-fix
    EXPECT_EQ(data[1], "event_a");
    EXPECT_EQ(data[4], "1") << "event_a should have 1 condition";
    EXPECT_EQ(data[5], "1") << "event_a should have 1 action";
    EXPECT_EQ(data[7], "cond_comment_0")
        << "condition_list[0][0].comment mismatch – was the cond->condition_list[N][0] fix applied?";
    EXPECT_EQ(data[8], "act_comment_0")
        << "action_list[0][0].comment mismatch – was the act->action_list[N][0] fix applied?";
}

// ---------------------------------------------------------------------------
// Test 3 – CyclicConditionList
//
// Subscribe cyclically to condition_list[0] fields on event_a.
// Before the fix, the var server would reject these paths because the field
// was still named "cond" internally. Successful subscription (non-empty
// numeric reply) proves the path resolves correctly.
// ---------------------------------------------------------------------------
TEST_F(MtvTest, CyclicConditionList)
{
    ASSERT_EQ(status, 0) << "Could not connect to variable server";

    // Subscribe to all four condition_list fields MtvApp.java uses.
    socket << "trick.var_add(\"trick_ip.mtv.mtv_list[0][0].condition_list[0][0].enabled\")\n" 
        "trick.var_add(\"trick_ip.mtv.mtv_list[0][0].condition_list[0][0].fired_time\")\n"
        "trick.var_add(\"trick_ip.mtv.mtv_list[0][0].condition_list[0][0].fired_count\")\n"
        "trick.var_add(\"trick_ip.mtv.mtv_list[0][0].condition_list[0][0].hold\")\n"
        "trick.var_cycle(0.1)\n";

    // Read one cycle of data; the var server should send back 4 values.
    std::string reply = socket.recv_line();
    auto tokens       = split_tabs(reply);

    // Message type 0 + 4 value tokens = 5 tokens minimum.
    ASSERT_GE(tokens.size(), 5u)
        << "condition_list subscription returned too few tokens.\n"
        << "Reply: " << reply << "\n"
        << "This indicates the variable path was not resolved (pre-fix 'cond' names).";

    EXPECT_EQ(tokens[0], "0") << "Expected var_send data message type (0)";

    // enabled defaults to true (1) for a freshly created condition.
    EXPECT_EQ(tokens[1], "1") << "condition_list[0].enabled should be 1 (true)";

    socket << "trick.var_clear()\n";
}

// ---------------------------------------------------------------------------
// Test 4 – CyclicActionList
//
// Same as Test 3 but for action_list[0] fields.
// ---------------------------------------------------------------------------
TEST_F(MtvTest, CyclicActionList)
{
    ASSERT_EQ(status, 0) << "Could not connect to variable server";

    socket << "trick.var_add(\"trick_ip.mtv.mtv_list[0][0].action_list[0][0].enabled\")\n"
        "trick.var_add(\"trick_ip.mtv.mtv_list[0][0].action_list[0][0].ran_time\")\n"
        "trick.var_add(\"trick_ip.mtv.mtv_list[0][0].action_list[0][0].ran_count\")\n"
        "trick.var_cycle(0.1)\n";

    std::string reply = socket.recv_line();
    auto tokens       = split_tabs(reply);

    // Message type 0 + 3 value tokens = 4 tokens minimum.
    ASSERT_GE(tokens.size(), 4u)
        << "action_list subscription returned too few tokens.\n"
        << "Reply: " << reply << "\n"
        << "This indicates the variable path was not resolved (pre-fix 'act' names).";

    EXPECT_EQ(tokens[0], "0") << "Expected var_send data message type (0)";
    EXPECT_EQ(tokens[1], "1") << "action_list[0].enabled should be 1 (true)";

    socket << "trick.var_clear()\n";
}

// ---------------------------------------------------------------------------
// Test 5 – UpdateTicker
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

    EXPECT_GT(ticker_after, ticker_before)
        << "mtv_update_ticker should have incremented after delete_event()";
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
