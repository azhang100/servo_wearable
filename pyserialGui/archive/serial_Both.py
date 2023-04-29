# from serial import *
from serial import Serial
from tkinter import *

deviceSerial = Serial("COM4", 9600, timeout=0, writeTimeout=0) #ensure non-blocking

WIDTH = 80

#============ make a TkInter Window ==============
root = Tk()
root.wm_title("Servoregulator Interface")

#============ Terminal ==============

Label(terminal, text="INCOMING").pack(side=TOP)

incomingFrame = Frame(terminal)
incoming = Text (incomingFrame, width=WIDTH, height=40, takefocus=0)
incoming.pack(side=LEFT)
incomingScroll= Scrollbar(incomingFrame) #scroll
incomingScroll.pack(side=RIGHT, fill = Y)
incoming.config(yscrollcommand=incomingScroll.set)
incomingScroll.config(command=incoming.yview)
incoming.bind("<Key>", lambda e: "break")
incomingFrame.pack(side=TOP)

Label(terminal, text="OUTGOING").pack(side=TOP)

outgoingFrame = Frame(terminal)
outgoing = Text (outgoingFrame, width=WIDTH, height=10, takefocus=0)
outgoing.bind("<Key>", lambda e: "break")
outgoing.pack(side=LEFT)
outgoingScroll = Scrollbar(outgoingFrame) #scroll
outgoingScroll.pack(side=RIGHT, fill = Y)
outgoing.config(yscrollcommand=outgoingScroll.set)
outgoingScroll.config(command=outgoing.yview)
outgoingFrame.pack(side=TOP)

Label(terminal, text="QUEUE").pack(side=TOP)

queueFrame = Frame(terminal)
queue = Text (queueFrame, width=WIDTH, height=10, takefocus=0)
queue.pack(side=LEFT)
queueScroll = Scrollbar(queueFrame) #scroll
queueScroll.pack(side=RIGHT, fill = Y)
queue.config(yscrollcommand=queueScroll.set)
queueScroll.config(command=queue.yview)
queueFrame.pack(side=TOP)

def sendWrapper():
    send()
sendButton = Button(terminal, text="Force Send", command=sendWrapper);
sendButton.pack(side=TOP);

#============ Graph TODO ==============



#============ functions ==============

def send():
    # 1) get the outputText
    outputText = queue.get("1.0",END)
    queue.delete("1.0",END)
    print("outputText:", outputText)
    print("outputText:", outputText.encode('UTF-8'))
    outputText = outputText + "\n"
    # 2) output on serial
    deviceSerial.write(outputText.encode('UTF-8'))
    # 3) display on screen
    outgoing.insert(END,outputText)
    outgoing.see(END)
    # 4) show log file
    f.write(outputText)
        
# serial
def readSerial():
    while True:
        c = deviceSerial.read().decode('UTF-8')
        if len(c) == 0: #was anything read?
            return
        fIn.write(c)
        incoming.insert(END, c)
        incoming.see(END)
    
def loop():
    readSerial()
    root.after(10, loop) # check serial again soon

#============ run + logging ==============

#======== start log =========

fOut = open("serialLogOut.txt", "a")
fOut.write("========new log========\n")

fIn = open("serialLogIn.txt", "a")
fIn.write("========new log========\n")

def onClosing():
    root.destroy()
    fOut.close()
    fIn.close()
    
# after initializing serial, an arduino may need a bit of time to reset
root.after(100, loop)
root.protocol("WM_DELETE_WINDOW", onClosing)
root.mainloop()

