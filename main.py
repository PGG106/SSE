import subprocess,os,lzma
p=None

f="./nn.net" if os.path.exists("./nn.net") else "/kaggle_simulations/agent/nn.net"
with lzma.open(f"{f}.xz", 'rb') as c:
    d=c.read()
with open(f, 'wb') as o:
    o.write(d)

f="./Alexandria" if os.path.exists("./Alexandria") else "/kaggle_simulations/agent/Alexandria"
with lzma.open(f"{f}.xz", 'rb') as c:
    d=c.read()
with open(f, 'wb') as o:
    o.write(d)
os.chmod(f, 0o777)

def chess_bot(obs):
    global p
    if not p or p.poll()!=None:
        p=subprocess.Popen(f,stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=1)
    t=int(obs.remainingOverageTime*1000)
    # Treat delay as inc
    p.stdin.write(f"position fen {obs.board}\ngo wtime {t} btime {t} winc 0 binc 0\n")
    p.stdin.flush()
    line = ""
    while True:
        line = p.stdout.readline()
        if line != "\n":
            break
    best_move = line.split()[-1]
    return best_move