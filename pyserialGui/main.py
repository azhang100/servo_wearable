
from tkinter import *

from terminal import *
from variables import *
from graph import *
from logger import *

class Master:
    def __init__(self, comPort):
    
        #============ make a TkInter Window ==============
        self.root = Tk()
        # self.root.geometry("750x250")
        self.root.wm_title("Servoregulator Interface")
        self.root.protocol("WM_DELETE_WINDOW", self.onClosing)
    
        #============ make features ==============
        
        self.terminal = Terminal(self, comPort)
        self.logger = Logger()
        
        self.variableManager = VariableManager(self)

        # must come after variableManager
        self.graphA = Graph(self, updatePeriod=100, xStep=50)
        self.graphB = Graph(self, updatePeriod=6000, xStep=50)
    
    def onClosing(self):
        self.root.destroy()
        self.terminal.close()
        self.logger.close()

class GetComPort():
    def __init__(self):
        self.win= Tk() # Create an instance of Tkinter frame
        self.win.geometry("300x100")
        #Initialize a Label to display the User Input
        label=Label(self.win, text="Enter Com Port")
        label.pack()
        #Create an Entry widget to accept User Input
        self.entry= Entry(self.win, width=40)
        self.entry.focus_set()
        self.entry.pack()
        #Create a Button to validate Entry Widget
        Button(self.win, text= "Okay",width= 20, command=self.accept).pack(pady=20)
        self.win.mainloop()
        
    def accept(self):
        self.result = self.entry.get()
        print("using:", self.result)
        self.win.destroy()
        
    def get(self):
        return self.result
        
#============== run TKInter ==========

comPort = GetComPort().get()
        
master = Master(comPort)
master.root.mainloop()
