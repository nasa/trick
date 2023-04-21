import sys

if len(sys.argv) != 3:
    print ("Usage: check_file_endings.py <ref_file> <cmp_file>")
    exit(1)

ref_file = sys.argv[1]
cmp_file = sys.argv[2]

ref_lines = reversed(list(open(ref_file)))
cmp_lines = reversed(list(open(cmp_file)))

for ref_line, cmp_line in zip(ref_lines, cmp_lines):
    if (ref_line != cmp_line):
        print("Difference in files: ", ref_line, "vs", cmp_line)
        exit(1)

print("Success!")
exit(0)

