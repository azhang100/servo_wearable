# from serial import *
from serial import Serial
from tkinter import *

class Terminal:
    
    def __init__(self, master, frame, portName):
    
        self.master = master
        self.root = master.root
        self.root.after(100, self.loop)
    
        #============ Serial ==============
    
        self.deviceSerial = Serial(portName, 9600, timeout=0, writeTimeout=0) #ensure non-blocking
        self.terminalFrameLeft = Frame(frame)
        self.terminalFrameLeft.grid(column=0, row=0)
        self.terminalFrameRight = Frame(frame)
        self.terminalFrameRight.grid(column=1, row=0)
        frame.grid_columnconfigure(0, weight=1)
        frame.grid_columnconfigure(1, weight=1)
                
        #============ Log ==============
        
        self.fOut = open("serialLogOut.txt", "a")
        self.fOut.write("========new log========\n")

        self.fIn = open("serialLogIn.txt", "a")
        self.fIn.write("========new log========\n")
        
        #============ TkInter ==============

        Label(self.terminalFrameLeft, text="INCOMING").pack(side=TOP)

        incomingFrame = Frame(self.terminalFrameLeft)
        self.incoming = Text (incomingFrame, height=44, width=40, takefocus=0)
        self.incoming.bind("<Key>", lambda e: "break")
        self.incoming.pack(side=LEFT, expand=True, fill='both')
        incomingScroll= Scrollbar(incomingFrame) #scroll
        incomingScroll.pack(side=RIGHT, fill = Y)
        self.incoming.config(yscrollcommand=incomingScroll.set)
        incomingScroll.config(command=self.incoming.yview)
        incomingFrame.pack(side=TOP, expand=True, fill='both')

        Label(self.terminalFrameRight, text="OUTGOING").pack(side=TOP)

        outgoingFrame = Frame(self.terminalFrameRight)
        self.outgoing = Text (outgoingFrame, height=40, width=40, takefocus=0)
        self.outgoing.bind("<Key>", lambda e: "break")
        self.outgoing.pack(side=LEFT, expand=True, fill='both')
        outgoingScroll = Scrollbar(outgoingFrame) #scroll
        outgoingScroll.pack(side=RIGHT, fill = Y)
        self.outgoing.config(yscrollcommand=outgoingScroll.set)
        outgoingScroll.config(command=self.outgoing.yview)
        outgoingFrame.pack(side=TOP, expand=True, fill='both')

        Label(self.terminalFrameRight, text="SEND").pack(side=TOP)

        queueFrame = Frame(self.terminalFrameRight)
        self.queue = Entry(queueFrame, takefocus=0, width=45)
        self.queue.pack(side=LEFT)
        queueFrame.pack(side=TOP)

        sendButton = Button(self.terminalFrameRight, text="Send", command=self._sendButton)
        sendButton.pack(side=TOP)

    #============ functions ==============

    def _sendButton(self):
        # 1) get the outputText
        outputText = self.queue.get("1.0",END)
        self.queue.delete("1.0",END)
        print("outputText:", outputText)
        print("outputText:", outputText.encode('UTF-8'))
        outputText = outputText + "\n"
        
        # 2) send
        self.send(outputText)

    def send(self, outputText):
        print("terminal.send("+outputText+")")
        # 1) output on serial
        self.deviceSerial.write(outputText.encode('UTF-8'))
        # 2) display on screen
        self.outgoing.insert(END,outputText)
        self.outgoing.see(END)
        # 3) print to log file
        self.fOut.write(outputText)
            
    # serial
    def _readSerial(self):
        while True:
            c = self.deviceSerial.read().decode('UTF-8')
            if len(c) == 0: #was anything read?
                return
            
            # write to log file
            self.fIn.write(c)
            
            # show on terminal
            self.incoming.insert(END, c)
            self.incoming.see(END)
            
            # update variables
            self.master.variableManager.incoming(c)
            self.master.logger.incoming(c)
        
    def loop(self):
        self._readSerial()
        self.root.after(10, self.loop) # check serial again soon

    def close(self):
        self.fOut.close()
        self.fIn.close()
        

