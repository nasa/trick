import pytest

from main import QuoteForPOSIX

def test_posix_quote():
    q = QuoteForPOSIX("hello")
    assert q == "'hello'"

    q = QuoteForPOSIX("hello world")
    assert q == "'hello world'"

    q = QuoteForPOSIX("\n")
    assert q == "'\n'"

    q = QuoteForPOSIX("hello world $x")
    assert q == "'hello world $x'"