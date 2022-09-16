# boards.txt auditing and repair tool

import re, json, shutil
from pprint import pprint

# every key/value in the file
kv = {}

# key/value pairs under each board
bkv = {}

# key before a given key
# used for insert locations
bkv_prev = {}

# list of boards that have a given key/value
bvh = {}


# generates the above index
def index_boards(fname):
    global kv, bkv, bkv_prev, bvh
    kv = {}
    bkv = {}
    bkv_prev = {}
    bvh = {}
    ln = 0
    lb = ""
    lbk = ""
    with open(fname) as f:
        for l in f:
            ln += 1
            if not len(l):  # empty
                pass
            elif l[0] == "#":  # comment
                pass
            else:
                m = re.match("([a-zA-Z_._0-9-]*)=(.*)", l)
                if m:  # key/value
                    k = m.group(1)
                    v = m.group(2)
                    if k in kv:  # often a copy/paste error
                        print(
                            "DUPLICATE value for", k, "at", ln, "old:", kv[k], "new", v
                        )
                    kv[k] = v
                    m2 = re.match("([a-z0-9]*)\.name", k)
                    if m2:  # a board name is introduced
                        bn = m2.group(1)
                        if bn in bkv:
                            print(
                                "DUPLICATE board definition for",
                                k,
                                "at",
                                ln,
                                "old:",
                                kv[k],
                                "new",
                                v,
                            )
                        else:
                            bkv[bn] = {}
                            bkv_prev[bn] = {}
                    m3 = re.match("([a-z0-9]*)\.(.*)", k)
                    if (
                        m3
                    ):  # a key-value that might be under a board (intentionally re-handling)
                        maybe_bn = m3.group(1)
                        if maybe_bn in bkv:
                            bk = m3.group(2)
                            bkv[maybe_bn][bk] = v
                            if not bk in bvh:
                                bvh[bk] = []
                            bvh[bk].append(maybe_bn)
                            if maybe_bn == lb:
                                bkv_prev[maybe_bn][bk] = lbk
                                lbk = bk
                            else:
                                lb = maybe_bn
                                lbk = k
                                bkv_prev[maybe_bn][bk] = None
                else:
                    m4 = re.match("\s*", l)  # whitespace
                    if not m4:
                        print("NOPARSE", ln, l)


# boards.out will contain the final results.
# because of fixpoint related issues multiple passes are needed.
# each pass uses the previous pass's output

# pass 0 starts with boards.txt
shutil.copy("../../boards.txt", "../../boards.out")
index_boards("../../boards.out")

# for manual inspection
ks = [k for k in bvh]
ks.sort()
for k in ks:
    # print(k,len(bvh[k]),' '.join(bvh[k]))
    pass


# pprint(bkv_prev)


# fixup is a newline delimited list of board-key=value lines
# fixwhere is a single key to check
# runs until it stops inserting things
# in each iteration, ../../boards.out is moved to ../../boards.in and then processed into ../../boards.out
def do_fixup(fixup_s, fixwhere):
    shutil.move("../../boards.out", "../../boards.in")
    index_boards("../../boards.in")
    global bvh, bvk, bkv_prev
    fixup = [e for e in fixup_s.split("\n") if len(e)]
    wants = [e.split("=")[0] for e in fixup]
    inserts = {}

    for e in fixup:
        bk = e.split("=")[0]
        if not bk in bvh:
            print("can't find", e, "in any board")
            exit(1)
        print(bkv_prev[bvh[bk][0]][bk])
        inserts[bkv_prev[bvh[bk][0]][bk]] = e

    pprint(inserts)
    dirty = 0

    with open("../../boards.in") as f:
        with open("../../boards.out", "w") as f2:
            for l in f:
                f2.write(l)
                m = re.match("([a-z0-9]*)\.([^=]*)=(.*)", l)
                if m:
                    m_bn = m.group(1)
                    bk = m.group(2)
                    v = m.group(3)
                    if m_bn in bkv and bk in inserts:
                        # print(bk in inserts, m_bn,bk,v)
                        # print(m_bn in bkv , bk in inserts , bk not in bkv[m_bn])
                        inskey = inserts[bk].split("=")[0]
                        if not inskey in bkv[m_bn]:
                            dirty += 1
                            print(m_bn + "." + inserts[bk])
                            print(m_bn + "." + inserts[bk], file=f2)
    if dirty:
        # some changes insert predecessors that trigger more changes
        print("made", dirty, "insertions, trying again")
        do_fixup(fixup_s, fixup)
    else:
        print("done")


# fixup is a list of things to insert where missing and for boards that have the key in fixwhere
fixup = """
menu.clock.2internal=2 MHz internal
menu.clock.2internal.build.speed=2
menu.clock.2internal.build.clocksource=0"""
# this makes the fixup conditional - clko vs clk
fixwhere = "menu.clock.1internal"

do_fixup(fixup, fixwhere)

fixup = """
menu.clocko.2internal=2 MHz internal
menu.clocko.2internal.build.speed=2
menu.clocko.2internal.build.clocksource=0"""
fixwhere = "menu.clocko.1internal"

do_fixup(fixup, fixwhere)
