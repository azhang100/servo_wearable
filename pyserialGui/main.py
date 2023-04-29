
from tkinter import *

from terminal import *
from variables import *
from graph import *

class Master:
    def __init__(self):
    
        #============ make a TkInter Window ==============
        self.root = Tk()
        self.root.wm_title("Servoregulator Interface")
        self.root.protocol("WM_DELETE_WINDOW", self.onClosing)
    
        #============ make features ==============
        
        self.terminal = Terminal(self, "COM1")
        
        self.variableManager = VariableManager(self)

        self.graph = Graph(self) # must come after variableManager
    
    def onClosing(self):
        self.root.destroy()
        self.terminal.close()
        
#============== run TKInter ==========

master = Master()
master.root.mainloop()
