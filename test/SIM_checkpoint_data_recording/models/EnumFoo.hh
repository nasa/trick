/*
PURPOSE: ( Scoped enums with every underlying integer type for data recording tests. )
*/

#include <cstdint>
#include <type_traits>

class EnumFoo
{
    public:
        enum class UC : uint8_t
        {
            a
        };
        enum class US : uint16_t
        {
            a
        };
        enum class UI : uint32_t
        {
            a
        };
        enum class UL : uint64_t
        {
            a
        };
        enum class SC : int8_t
        {
            a
        };
        enum class SS : int16_t
        {
            a
        };
        enum class SI : int32_t
        {
            a
        };
        enum class SL : int64_t
        {
            a
        };

        UC uc;
        US us;
        UI ui;
        UL ul;
        SC sc;
        SS ss;
        SI si;
        SL sl;

        // Unsigned values start above the signed max of their width, signed values are negative,
        // and the 8-byte values do not fit in 4 bytes, so a wrong width or signedness shows in the log.
        // 4-byte enums are recorded as int regardless of signedness, so ui stays within the int range.
        void init()
        {
            uc = static_cast<UC>(200);
            us = static_cast<US>(40000);
            ui = static_cast<UI>(2000000000u);
            ul = static_cast<UL>(5000000000ull);
            sc = static_cast<SC>(-100);
            ss = static_cast<SS>(-30000);
            si = static_cast<SI>(-2000000000);
            sl = static_cast<SL>(-5000000000ll);
        }

        void increment()
        {
            step(uc, 1);
            step(us, 1);
            step(ui, 1);
            step(ul, 1);
            step(sc, -1);
            step(ss, -1);
            step(si, -1);
            step(sl, -1);
        }

    private:
        template <class T> void step(T& t, int delta)
        {
            t = static_cast<T>(static_cast<std::underlying_type_t<T>>(t) + delta);
        }
};
