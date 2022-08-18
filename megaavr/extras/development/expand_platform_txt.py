# expands platform.txt.in to produce platform.txt on stdout
# stores values defined by versionnum.x=y
# replaces the line version={versionnum.x}...{versionnum.y}... with the values above

import re, sys

vars = {}
with open("platform.txt.in") as f:
    for l in f:
        l = l.strip()
        m = re.match("[ ]*versionnum.([a-z]*)[ ]*=[ ]*(.*)[ ]*", l)
        if m:
            if m.group(1) in vars:
                print("duplicate definition of:", m.group(1), file=sys.stderr)
                exit(1)
            else:
                vars[m.group(1)] = m.group(2)
        m = re.match("([ ]*version=)(.*)", l)
        if m:  # not elseif, want the first case to fall through
            exp = m.group(1) + m.group(2)
            for k in vars:
                exp = exp.replace("{versionnum." + k + "}", vars[k])
            if exp.find("{") >= 0 or exp.find("}") >= 0:
                print(
                    '"' + exp + '"',
                    "still contains { or }, define the value above",
                    file=sys.stderr,
                )
                exit(1)
            print(exp)
        else:  # not the line being changed
            print(l)
