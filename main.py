import subprocess, os, lzma

p = None
pondermove = None

f = "./nn.net" if os.path.exists("./nn.net") else "/kaggle_simulations/agent/nn.net"
with lzma.open(f"{f}.xz", 'rb') as c:
    d = c.read()
with open(f, 'wb') as o:
    o.write(d)

f = "./sse" if os.path.exists("./sse") else "/kaggle_simulations/agent/sse"
with lzma.open(f"{f}.xz", 'rb') as c:
    d = c.read()
with open(f, 'wb') as o:
    o.write(d)
os.chmod(f, 0o777)


def chess_bot(b):
    global p
    global pondermove
    if not p or p.poll() != None:
        p = subprocess.Popen(f, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=1)
    t = int(b.remainingOverageTime * 1000)
    # Treat delay as inc
    if b.opponentMove == pondermove:
        p.stdin.write(f"ponderhit\n")
    else:
        p.stdin.write(f"pondermiss\n")
    p.stdin.write(f"position fen {b.board}\ngo wtime {t} btime {t} winc 0 binc 0\n")
    p.stdin.flush()
    l = ""
    while True:
        l = p.stdout.readline()
        if l != "\n":
            break
    pondermove = l.split()[-1]
    return l.split()[-3]
