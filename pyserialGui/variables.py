
from tkinter import *

#============ Variables TODO ==============

class Variable:
    def __init__(self, master, name, rw, minDispVal, maxDispVal):
        self.master = master
        self.name = name
        self.canRead = (rw == "R") or (rw == "RW")
        self.canWrite = (rw == "W") or (rw == "RW")
        self.userAccessible = False
        self.val = "N/A"
        
        self.labelName = "TODO"
        self.labelRead = "TODO"
        self.textWrite = "TODO"
        self.buttonWrite = "TODO"
        
        self.minDispVal = minDispVal
        self.maxDispVal = maxDispVal
        
    def updateRead(self, val):
        self.val = val
        self.labelRead.config(text = str(val))
        
    def send(self):
        sendVal = self.textWrite.get("1.0",END)
        if isdigit(sendVal):
            self.master.terminal.send("["+self.name+":"+sendVal+"]")
        
    def setupGui(self, variableFrame, index):
        Label(variableFrame, width=20, text=self.name).grid(row=index, column=0)
            
        self.labelRead = Label(variableFrame, text=self.val, width=10)
        self.labelRead.grid(row=index, column=1)
        Button(variableFrame, text="Graph", command=self.toggleGraph).grid(row=index, column=2)
        
        if self.canWrite:
            self.textWrite = Text(variableFrame, width=10, height=1)
            self.textWrite.grid(row=index, column=3)
            Button(variableFrame, text="Send", command=self.send).grid(row=index, column=4)
        else:
            Label(variableFrame, text="read only", height=1).grid(row=index, column=3)
            Label(variableFrame, text="", height=1).grid(row=index, column=4)
    
    def toggleGraph(self):
        self.master.graph.toggleVariable(self.name)
    
#    def __init__(self, name, rw, minDispVal, maxDispVal):
#        self.__init__(name, rw)
#        self.minDispVal = minDispVal
#        self.maxDispVal = maxDispVal
#        self.graph = FALSE

class VariableManager:
    def __init__(self, master):
        self.master = master
        self.root = master.root
        self.setupVariables()
        
        self.variableFrame = Frame(self.root)
        
        Label(self.variableFrame, text="NAME", font=('Helvetica 12 bold')).grid(row=0, column=0)
        Label(self.variableFrame, text="READ", font=('Helvetica 12 bold')).grid(row=0, column=1)
        Label(self.variableFrame, text="WRITE", font=('Helvetica 12 bold')).grid(row=0, column=3)
        index = 0
        for key in self.variables:
            index += 1
            self.variables[key].setupGui(self.variableFrame, index)
        
        self.variableFrame.pack(side=RIGHT)
        self.incomingString = ""

    def addVariable(self, name, rw, minDispVal=0, maxDispVal=100):
        self.variables[name] = Variable(self.master,name,rw, minDispVal, maxDispVal)

    def setupVariables(self):
        self.variables = {}
        
        self.addVariable("egco2","R")
        self.addVariable("pressureChamber","R")
        self.addVariable("humidity","R")
        self.addVariable("temp","R")
        self.addVariable("tTemp","RW")
        self.addVariable("tegco2","RW")

        # servoWearable only
        self.addVariable("suggestSweep", "R", 0, 10000)
        self.addVariable("tSweep", "R", 0, 10000)
        self.addVariable("sweep", "R", 0, 10000)
        self.addVariable("sweepP", "RW", 0, 10000)
        self.addVariable("sweepI", "RW", 0, 10000)
        self.addVariable("sweepD", "RW", 0, 10000)
        self.addVariable("pressureInput", "R")
        self.addVariable("systemVersion", "R")

        # servoVA only # TODO
        
    def incoming(self, msg):
        for char in msg:
            if (char=="["):
                self.incomingString = ""
            elif (char=="]"):
                print("Got Msg: ["+self.incomingString+"]")
                incomingStringSplit = self.incomingString.split("=")
                print(incomingStringSplit)
                if (len(incomingStringSplit) == 2):
                    name = incomingStringSplit[0]
                    val = incomingStringSplit[1]
                    # if (isDigit(incomingStringSplit[1])):
                    if (True): # TODO isDigit properly
                        print("name:", name, ", val:", val)
                        self.variables[name].updateRead(int(val))
                self.incomingString = ""
            else:
                self.incomingString = self.incomingString + char
        
        
        