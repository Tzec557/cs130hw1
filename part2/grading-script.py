#!/usr/bin/python

# ./grading-script.py <test-dir>

import os,re,sys,shutil,random,subprocess,threading

import sys
import os

def Usage():
    print("Usage: python your_script.py <test_dir>")
    sys.exit(1)
    

##########################
# Check that exactly one argument was provided (excluding the script name)
if len(sys.argv) != 2:
    print("Error: Please provide exactly one argument specifying the directory containing 'scheme.txt'.")
    Usage()

test_dir = sys.argv[1]

# Validate that the argument is a directory
if not os.path.isdir(test_dir):
    print("Error: '%s' is not a valid directory." % test_dir)
    Usage()

# Validate that the directory contains 'scheme.txt'
scheme_file_path = os.path.join(test_dir, "scheme.txt")
if not os.path.isfile(scheme_file_path):
    print("Error: The directory '%s' does not contain a file named 'scheme.txt'." % test_dir)
    Usage()
##########################

test_dir=sys.argv[1]

# Set up scratch space for grading
dir="grading"
try:
    shutil.rmtree(dir)
except:
    pass
os.mkdir(dir)

re_cp=re.compile(r'\.h$|\.cpp$|^SConstruct$|\.obj$$|\.png$')
for f in os.listdir('.'):
    if re_cp.search(f):
        shutil.copyfile(f, dir+"/"+f)

# Discourage cheating
token='TOKEN'+str(random.randrange(100000,999999))

if subprocess.call(['scons'],cwd=dir)!=0:
    print("FAIL: Did not compile")
    exit()

def run_command_with_timeout(cmd, timeout_sec):
    proc = subprocess.Popen(cmd,cwd=dir)
    proc_thread = threading.Thread(target=proc.communicate)
    proc_thread.start()
    proc_thread.join(timeout_sec)
    if proc_thread.is_alive():
        try:
            proc.kill()
        except OSError as e:
            return True
        return False
    return True

hashed_tests={}
total_score=0

ignore_line=re.compile(r'^\s*(#|$)')
grade_line=re.compile(r'^(\S+)\s+(\S+)\s+(\S+)\s*$')
gs=0
try:
    gs=open(test_dir+'/scheme.txt')
except:
    print("FAIL: could not open grading scheme.")
    exit()

diff_parse=re.compile('diff: (.*)')
grade_cmd=['./ray_tracer', '-i', 'file.txt', '-s', 'file.png', '-f', token+'.txt']
grade_cmd.extend(sys.argv[2:])

for line in gs.readlines():
    if ignore_line.search(line):
        continue
    g=grade_line.search(line)
    if not g:
        print("Unrecognized command: "+line)
        exit()
    points=float(g.groups()[0])
    max_error=float(g.groups()[1])
    file=g.groups()[2]

    pass_error = 0
    pass_time = 0
    if not file in hashed_tests:
        timeout = 10
        shutil.copyfile(file, dir+"/file.txt")
        shutil.copyfile(file.replace("txt","png"), dir+"/file.png")
        if not run_command_with_timeout(grade_cmd, timeout):
            hashed_tests[file]="TIMEOUT"
        else:
            try:
                report = dir+'/'+token+'.txt'
                results_file=open(report)
                d=diff_parse.match(results_file.readline())
                results_file.close()
                if os.path.isfile(report):
                    os.remove(report) # remove the diff file
                if d: d=float(d.groups()[0])
                hashed_tests[file]=d
            except:
                hashed_tests[file]=None

    d=hashed_tests[file]
    if d=="TIMEOUT":
        print("FAIL: (%s) Test timed out."%file)
        points=0
    elif d==None:
        print("FAIL: (%s) Program failed to report statistics."%file)
        points=0
    else:
        if d>max_error:
            print("FAIL: (%s) Too much error. Actual: %g  Max: %g."%(file,d,max_error))
            points=0
        else:
            print("PASS: (%s) diff %g vs %g."%(file,d,max_error))

    if points>0:
        print("+%g points"%points)
        total_score+=points
    else:
        print("no points")

print("FINAL SCORE: %g"%total_score)

