
from tkinter import *

#============ Variables TODO ==============

class Variable:
    def __init__(self, master, name, rw):
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
        
    def updateRead(self, val):
        self.val = val
        self.labelRead.config(text = String(val))
        
    def send(self):
        self.master.terminal.send("["+self.name+":"+String(self.val)+"]")
        
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
        
        for key in self.variables:
            variable = self.variables[key]
            
            individualFrame = Frame(self.variableFrame)
            individualFrame.pack(side=TOP)
            
            Label(self.variableFrame, text=variable.name).pack(side=LEFT)
            variable.labelName = Label(individualFrame, width=20, text=variable.name)
            variable.labelName.pack(side=LEFT)
            
            variable.labelRead = Label(individualFrame, text=variable.val, width=10)
            if variable.canWrite:
                variable.textWrite = Text(individualFrame, width=20, height=1)
                variable.textWrite.pack(side=LEFT)
                variable.buttonWrite = Button(individualFrame, text="Send", command=variable.send);
                variable.buttonWrite.pack(side=LEFT);
        
        self.variableFrame.pack(side=RIGHT)
        self.incomingString = ""

    def addVariable(self, name, rw):
        self.variables[name] = Variable(self.master,name,rw)

    def setupVariables(self):
        self.variables = {}
        
        self.addVariable("egco2","R")
        self.addVariable("pressureChamber","R")
        self.addVariable("humidity","R")
        self.addVariable("temp","R")
        self.addVariable("tTemp","RW")
        self.addVariable("tEgco2","RW")

        # servoWearable only
        self.addVariable("suggestSweep", "R")
        self.addVariable("tSweep", "R")
        self.addVariable("sweep", "R")
        self.addVariable("sweepP", "RW")
        self.addVariable("sweepI", "RW")
        self.addVariable("sweepD", "RW")
        self.addVariable("pressureInput", "R")
        self.addVariable("systemVersion", "R")

        # servoVA only # TODO
        
    def incoming(self, msg):
        for char in msg:
            if (char=="["):
                self.incomingString = ""
            elif (char=="]"):
                incomingStringSplit = incomingString.split(":")
                if (len(incomingString) == 2):
                    name = incomingStringSplit[0]
                    val = incomingStringSplit[1]
                    if (isDigit(incomingString[1])):
                        variableDict[name].updateRead(val)
            else:
                self.incomingString = self.incomingString + char
        
        
        