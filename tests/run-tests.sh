#!/bin/bash
#******************************************************************************
# FILE:    tests/run-tests.sh
# LANGUAGE: bash
# DESCRIPTION
#     Basic regression tests for the tools in this repository.  One or more
#     checks per built command: deterministic commands are checked against an
#     exact expected output, non-deterministic ones (RNG / date / PostScript)
#     get a smoke check (exit status and/or non-empty output).
#
#     Run from anywhere after building the tree:
#         make PREFIX=/usr/local depend all
#         sh tests/run-tests.sh
#
#     Exit status is 0 only if every non-skipped check passes.
# AUTHORS
#     <PJB> Pascal J. Bourguignon (tools); test harness added during review.
#******************************************************************************
B="$(cd "$(dirname "$0")/.." && pwd)"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT
PASS=0 FAIL=0 SKIP=0
FAILED_NAMES=""

ok()   { PASS=$((PASS+1)); printf 'PASS  %s\n' "$1"; }
ko()   { FAIL=$((FAIL+1)); FAILED_NAMES="$FAILED_NAMES\n    $1"; printf 'FAIL  %s\n' "$1";
         [ -n "$2" ] && printf '        expected: %s\n        got:      %s\n' "$2" "$3"; }
skip() { SKIP=$((SKIP+1)); printf 'SKIP  %s  (%s)\n' "$1" "$2"; }

# assert_eq NAME EXPECTED ACTUAL
assert_eq() { if [ "$2" = "$3" ]; then ok "$1"; else ko "$1" "$2" "$3"; fi; }
# assert_rc NAME EXPECTED_CODE ACTUAL_CODE
assert_rc() { if [ "$2" = "$3" ]; then ok "$1"; else ko "$1" "rc=$2" "rc=$3"; fi; }
# assert_nonempty NAME ACTUAL
assert_nonempty() { if [ -n "$2" ]; then ok "$1"; else ko "$1" "<non-empty>" "<empty>"; fi; }
# assert_contains NAME NEEDLE HAYSTACK
assert_contains() { case "$3" in *"$2"*) ok "$1";; *) ko "$1" "*$2*" "$3";; esac; }

# hex NAME: read stdin, emit lowercase hex bytes with single spaces
hexdump_bytes() { od -An -tx1 | tr -s ' \n' ' ' | sed -e 's/^ //' -e 's/ $//'; }

echo "=== tools test suite ==="
echo "repo: $B"
echo

# ---------------------------------------------------------------- ascii-utilities
A="$B/ascii-utilities/sources"
assert_eq "ascii/binskip skip 2"        "CDEF" "$(printf 'ABCDEF' | "$A/binskip" 2)"
assert_eq "ascii/bintohex AB"           "4142" "$(printf 'AB' | "$A/bintohex" | tr -d '\n')"
assert_eq "ascii/hextobin 4142"         "41 42" "$(printf '4142' | "$A/hextobin" | hexdump_bytes)"
assert_eq "ascii/beep emits BEL"        "07" "$("$A/beep" </dev/null | hexdump_bytes)"
assert_eq "ascii/capitalize args"       "$(printf 'Hello\nWorld')" "$("$A/capitalize" hello world </dev/null)"
assert_eq "ascii/lc -m CRLF"            "48 65 6c 6c 6f 0d" "$(printf 'Hello\n' | "$A/lc" -m | hexdump_bytes)"
assert_eq "ascii/sevenbit ascii id"    "abc" "$(printf 'abc' | "$A/sevenbit")"
assert_eq "ascii/text strips ctrl"      "abc" "$(printf 'a\000b\007c' | "$A/text")"
assert_eq "ascii/sortchars"             "abcd" "$(printf 'dcba' | "$A/sortchars")"
assert_eq "ascii/parity -s -o on A"     "c1" "$(printf 'A' | "$A/parity" -s -o | hexdump_bytes)"
assert_eq "ascii/charcount counts"      "2" "$(printf 'AAB' | "$A/charcount" | awk '$0 ~ /^41 /{print $NF}')"
assert_contains "ascii/ascii table"     "A" "$("$A/ascii" </dev/null)"
assert_eq "ascii/whatisit binary"       "Binary" "$(printf '\000\001\377' > "$TMP/bin.dat"; "$A/whatisit" "$TMP/bin.dat" | awk '{print $NF}')"
assert_eq "ascii/whatisit ascii"        "Ascii" "$(printf 'hello\n' > "$TMP/asc.dat"; "$A/whatisit" "$TMP/asc.dat" | awk '{print $NF}')"
assert_eq "ascii/encoding -A clears hi" "41 42" "$(printf '\301\302' | "$A/encoding" -A | hexdump_bytes)"
assert_eq "ascii/randchar -r 65 65"     "AAAAAAAA" "$(timeout 5 "$A/randchar" -c 8 -r 65 65)"
assert_eq "ascii/randchar -n -r 90 90"  "ZZZZZ" "$(timeout 5 "$A/randchar" -n -c 5 -r 90 90)"

# ---------------------------------------------------------------- base
assert_eq "base 255 -> hex FF"          "FF" "$("$B/base/sources/base" 255 16 </dev/null)"
assert_eq "base 10 -> bin 1010"         "1010" "$("$B/base/sources/base" 10 2 </dev/null)"

# ---------------------------------------------------------------- romain
ROM="$("$B/romain/sources/romain" </dev/null)"
assert_contains "romain has I"          "1 = I" "$ROM"
assert_contains "romain has HORS-LIMITE" "HORS-LIMITE" "$ROM"

# ---------------------------------------------------------------- primes
assert_eq "primes to 30"                "$(printf '2\n3\n5\n7\n11\n13\n17\n19\n23\n29')" "$("$B/primes/sources/primes" to 30 </dev/null)"

# ---------------------------------------------------------------- sum32
printf 'hello' > "$TMP/h.txt"
assert_eq "sum32 of 'hello'"            "478560413032" "$("$B/sum32/sources/sum32" "$TMP/h.txt" | awk '{print $1}')"

# ---------------------------------------------------------------- sumascii
assert_contains "sumascii hello sum"    "532" "$("$B/sumascii/sumascii" hello </dev/null)"

# ---------------------------------------------------------------- mpa
assert_contains "mpa alias jdoe"        "JohnDoe:jdoe" "$("$B/mpa/sources/make-person-aliases" John Doe : jdoe </dev/null)"

# ---------------------------------------------------------------- newmoon (non-det: date/term)
"$B/newmoon/sources/newmoon" </dev/null >/dev/null 2>&1; assert_rc "newmoon runs" 0 $?

# ---------------------------------------------------------------- random-stat (non-det: smoke)
assert_contains "random-stat header"    "0-seq" "$("$B/random-stat/sources/random-stat" </dev/null)"

# ---------------------------------------------------------------- newton (non-det float; smoke)
assert_nonempty "newton runs"           "$(printf '1 1\n1 0\n0 1\n0.01\n' | timeout 5 "$B/newton/sources/newton" 2>/dev/null | head -3)"
assert_nonempty "newton-ps runs"        "$(printf '10\n1\n0.1\n1\n0 1\n1 0\n0 1\n1\n0.01\n' | timeout 5 "$B/newton/sources/newton-ps" 2>/dev/null | head -3)"

# ---------------------------------------------------------------- segment / join roundtrip
yes "0123456789ABCDEF" | head -c 5000 > "$TMP/rt.ref"; cp "$TMP/rt.ref" "$TMP/rt"
( cd "$TMP" && "$B/segment/sources/segment" -s 1024 rt </dev/null && rm -f rt && "$B/segment/sources/join" rt </dev/null )
cmp -s "$TMP/rt" "$TMP/rt.ref"; assert_rc "segment+join roundtrip" 0 $?

# ---------------------------------------------------------------- unsparse (fixed: keeps fname)
printf 'ABC\n' > "$TMP/u.dat"; cp "$TMP/u.dat" "$TMP/u.ref"
"$B/segment/sources/unsparse" "$TMP/u.dat" </dev/null 2>"$TMP/u.err"; URC=$?
cmp -s "$TMP/u.dat" "$TMP/u.ref"; CRC=$?
if [ "$URC" = 0 ] && [ "$CRC" = 0 ] && [ ! -s "$TMP/u.err" ]; then ok "unsparse no-op preserves file"; else ko "unsparse no-op preserves file" "rc0,same,noerr" "rc=$URC cmp=$CRC err=$(cat "$TMP/u.err")"; fi

# ---------------------------------------------------------------- line-utilities
L="$B/line-utilities/sources"
assert_eq "lrev reverses lines"         "$(printf 'three\ntwo\none')" "$(printf 'one\ntwo\nthree\n' | timeout 5 "$L/lrev")"
printf 'a1\na2\n' > "$TMP/A.txt"; printf 'b1\nb2\n' > "$TMP/B.txt"
assert_eq "lcat side-by-side"           "$(printf 'a1b1\na2b2\n')" "$(timeout 5 "$L/lcat" "$TMP/A.txt" "$TMP/B.txt")"
printf 'aaaa\nbbb\nbbb\nccc\nddd\n' > "$TMP/d1.txt"; printf 'bbb\nccc\neee\n' > "$TMP/d2.txt"
assert_eq "ldiff set difference"        "$(printf 'aaaa\nddd')" "$(timeout 5 "$L/ldiff" "$TMP/d1.txt" "$TMP/d2.txt")"
printf 'a|1\nb|2\nc|3\n' > "$TMP/f1.txt"; printf 'b|x\nc|y\n' > "$TMP/f2.txt"
assert_eq "fdiff field difference"      "a|1" "$(timeout 5 "$L/fdiff" "$TMP/f1.txt" "$TMP/f2.txt")"
printf 'apple|red\nbanana|yellow\ncherry|dark\n' > "$TMP/kv.txt"; printf 'apple\ncherry\n' > "$TMP/k.txt"
assert_eq "lgetvalue lookup"            "$(printf 'red\ndark')" "$(timeout 5 "$L/lgetvalue" "$TMP/kv.txt" "$TMP/k.txt")"
printf 'abc\n' | timeout 5 "$L/lmax" 100 >/dev/null 2>&1; assert_rc "lmax all-short -> 0" 0 $?
printf 'abcdefghij\n' | timeout 5 "$L/lmax" 3 >/dev/null 2>&1; assert_rc "lmax all-long -> 2" 2 $?
printf 'ab\nabcdefghij\n' | timeout 5 "$L/lmax" 3 >/dev/null 2>&1; assert_rc "lmax mixed -> 1" 1 $?

# ---------------------------------------------------------------- extractbit1
assert_eq "extractbit1 LSB pack"        "aa" "$(printf '\001\000\001\000\001\000\001\000' | timeout 5 "$B/extractbit1/sources/extractbit1" | hexdump_bytes)"

# ---------------------------------------------------------------- truncate
printf 'HelloWorld' > "$TMP/trunc.dat"; "$B/truncate/sources/truncate" -4 "$TMP/trunc.dat" </dev/null
assert_eq "truncate to 4 bytes"         "Hell" "$(cat "$TMP/trunc.dat")"
assert_eq "truncate/binskip skip 5"     "World" "$(printf 'HelloWorld' | timeout 5 "$B/truncate/sources/binskip" 5)"

# ---------------------------------------------------------------- dosname
mkdir -p "$TMP/ds" "$TMP/dd"; : > "$TMP/ds/My Long File.txt"
assert_eq "dosname 8.3 name"            "mylonfil.txt" "$(timeout 5 "$B/dosname/sources/dosname" -d -p "$TMP/ds" "$TMP/dd" 2>/dev/null </dev/null | grep -o 'mylonfil.txt' | head -1)"

# ---------------------------------------------------------------- encoding
assert_eq "nema ascii passthrough"      "41 42 43" "$(printf 'ABC' | timeout 5 "$B/encoding/sources/nema" -from ascii -to ascii | hexdump_bytes)"
assert_nonempty "bentest -a list"       "$(timeout 5 "$B/encoding/sources/bentest" -a </dev/null)"

# ---------------------------------------------------------------- cookie (non-det / env)
printf '#\nHELLO\n#\nHELLO\n#\nHELLO\n#\n' > "$TMP/ck.data"
echo "$TMP/ck.data" > "$TMP/ck.files"
assert_eq "cookie/cookie deterministic" "HELLO" "$(cookiefiles="$TMP/ck.files" timeout 5 "$B/cookie/sources/cookie")"
assert_eq "cookie/random 1 -> 0"        "0" "$(timeout 5 "$B/cookie/sources/random" 1)"

# ---------------------------------------------------------------- bocrypt
printf 'PLAINTEXTDATA' > "$TMP/bp.txt"
"$B/bocrypt/sources/bocrypt" pass 0 "$B/bocrypt/sources/key1t" "$TMP/bp.txt" > "$TMP/bo.enc" 2>/dev/null
assert_eq "bocrypt xor roundtrip"       "PLAINTEXTDATA" "$("$B/bocrypt/sources/bocrypt" pass 0 "$B/bocrypt/sources/key1t" "$TMP/bo.enc" 2>/dev/null)"
printf 'mykey123' > "$TMP/xk.txt"
assert_eq "xor roundtrip"               "HELLO" "$(printf 'HELLO' | timeout 5 "$B/bocrypt/sources/xor" -k "$TMP/xk.txt" | timeout 5 "$B/bocrypt/sources/xor" -k "$TMP/xk.txt")"
if [ -f "$B/bocrypt/sources/key1t" ] && [ -f "$B/bocrypt/sources/test-1.txt" ]; then
  assert_nonempty "sencrypt produces output" "$(timeout 5 "$B/bocrypt/sources/sencrypt" -q "$B/bocrypt/sources/key1t" "$B/bocrypt/sources/test-1.txt" 2>/dev/null)"
else skip "bocrypt/sencrypt" "fixture key1t/test-1.txt missing"; fi

# ---------------------------------------------------------------- cplusinherited
printf 'B\tA\nC\tA\n' > "$TMP/inh.txt"
assert_eq "cplusinherited tree"         "$(printf ' : <ROOT>\n     : A\n         : B\n         : C')" "$(timeout 5 "$B/cplusinherited/sources/cplusinherited" -a "$TMP/inh.txt" </dev/null)"

# ---------------------------------------------------------------- formats / s2d
printf 'data' > "$TMP/myfile"
S2D_OUT="$( cd "$B/formats/single_to_double" && ./setfile -t TEXT -c ttxt "$TMP/myfile" >/dev/null 2>&1 && ./type "$TMP/%myfile" 2>/dev/null )"
assert_contains "s2d setfile/type TEXT" "TEXT" "$S2D_OUT"

# ---------------------------------------------------------------- formats / binhextools
BHT="$B/formats/binhextools/sources"
assert_eq "seven/unseven roundtrip"     "Hello, World!" "$(cd "$BHT" && printf 'Hello, World!' | timeout 5 ./seven | timeout 5 ./unseven | tr -d '\000')"
assert_eq "runlength roundtrip"         "AAAAAAAAAABBBCCCCCCCCCC" "$(cd "$BHT" && printf 'AAAAAAAAAABBBCCCCCCCCCC' | timeout 5 ./runlength | timeout 5 ./unrunlength)"
assert_contains "bh usage"              "Usage" "$(timeout 5 "$BHT/bh" </dev/null 2>&1)"

# ---------------------------------------------------------------- formats / macbinary
printf 'test data fork' > "$TMP/df.txt"
"$B/formats/macbinary/sources/mb" -c "$TMP/df.txt" -T TEXT -C ttxt > "$TMP/out.mb" 2>/dev/null
assert_contains "mb encode/info type"   "54584554" "$("$B/formats/macbinary/sources/mb" -t -f "$TMP/out.mb" </dev/null 2>/dev/null)"

# ---------------------------------------------------------------- od-tools
assert_eq "odhv2hex strips address"     "$(printf 'feedface00000006\ndeadbeef')" "$(printf '0000000     feed    face    0000    0006\n0000020     dead    beef\n' | timeout 5 "$B/od-tools/sources/odhv2hex")"
assert_eq "postodax joins cont lines"   "$(printf 'abc \tdef\nghi  jkl')" "$(printf 'abc\n\tdef\nghi\n jkl\n' | timeout 5 "$B/od-tools/sources/postodax")"

# ---------------------------------------------------------------- objc-tools
if [ -f "$B/objc-tools/sources/test.input" ] && [ -f "$B/objc-tools/sources/test.output" ]; then
  timeout 5 "$B/objc-tools/sources/objc-make-accessors" < "$B/objc-tools/sources/test.input" 2>/dev/null | diff -q - "$B/objc-tools/sources/test.output" >/dev/null
  assert_rc "objc-make-accessors fixture" 0 $?
else skip "objc-tools/objc-make-accessors" "fixtures test.input/test.output missing"; fi

# ---------------------------------------------------------------- network
assert_eq "network levels"              "$(printf '     3\tc\n     2\tb\tc\n     1\ta\tb')" "$(printf 'a b\nb c\n' | timeout 5 "$B/network/sources/network")"

# ---------------------------------------------------------------- geturls
# geturls writes the bare URL list to stderr and an HTML bookmark doc (stdout).
GU_OUT="$(printf 'see http://example.com/x and https://foo.bar/y\n' | timeout 5 "$B/geturls/sources/geturls" 2>/dev/null)"
GU_ERR="$(printf 'see http://example.com/x and https://foo.bar/y\n' | timeout 5 "$B/geturls/sources/geturls" 2>&1 1>/dev/null)"
assert_contains "geturls href http"     "href=\"http://example.com/x\"" "$GU_OUT"
assert_contains "geturls href https"    "href=\"https://foo.bar/y\"" "$GU_OUT"
assert_eq "geturls bare list (stderr)"  "$(printf 'http://example.com/x\nhttps://foo.bar/y')" "$GU_ERR"

# ---------------------------------------------------------------- filldisk (usage only; DO NOT run for real)
"$B/filldisk/sources/filldisk" </dev/null >/dev/null 2>&1; assert_rc "filldisk usage exits 1" 1 $?

# ---------------------------------------------------------------- environment
assert_eq "environment --sh"            "$(printf '#!/bin/sh\nexport NOTHING_BEFORE=1\nexport _NOTHING_BEFORE=1')" "$(timeout 5 "$B/environment/sources/environment" --sh < "$B/environment/sources/test-1.env")"

echo
echo "=== summary: $PASS passed, $FAIL failed, $SKIP skipped ==="
if [ "$FAIL" -ne 0 ]; then printf 'failed:%b\n' "$FAILED_NAMES"; exit 1; fi
exit 0
