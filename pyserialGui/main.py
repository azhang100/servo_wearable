
from tkinter import *
from tkinter import ttk

from terminal import *
from variables import *
from graph import *
from logger import *

class Master:
    def __init__(self, comPort):
    
        #============ make a TkInter Window ==============
        
        self.root = Tk()
        self.root.geometry("1280x800")
        #try:
        #    self.root.attributes("-zoomed", True) # only works on ubuntu
        #except:
        #    pass
        self.root.wm_title("Servoregulator Interface")
        self.root.protocol("WM_DELETE_WINDOW", self.onClosing)
        
        #============ make features ==============
        
        self.logger = Logger()

        variableFrame = Frame(self.root)
        self.variableManager = VariableManager(self, variableFrame)
        variableFrame.pack(fill=Y, expand=False, side=RIGHT, pady=20, padx=50)
        
        self.note = ttk.Notebook(self.root)
        self.note.pack(fill=BOTH, expand=True, side=RIGHT, pady=10)
        
        frame = Frame(self.note)
        self.terminal = Terminal(self, frame, comPort)
        frame.pack(fill="both", expand=True)
        self.note.add(frame, text="Terminal")
        
        # must come after variableManager
        graphFrame = Frame(self.note)
        frame = Frame(graphFrame)
        self.graphA = Graph(self, frame, updatePeriod=100, xStep=50)
        frame.grid(column=0, row=0, sticky=E)
        frame = Frame(graphFrame)
        self.graphB = Graph(self, frame, updatePeriod=6000, xStep=50)
        frame.grid(column=0, row=1, sticky=E)
        self.note.add(graphFrame, text="Graph")
    
    def onClosing(self):
        self.root.destroy()
        self.terminal.close()
        self.logger.close()

def readFile(fileName):
    try:
        f = open("demofile.txt", "r")
        res = f.read()
        f.close()
        return res
    except:
        f = open("demofile.txt", "w")
        f.write("")
        f.close()
        return ""
    
def saveFile(fileName, text):
    f = open("demofile.txt", "w")
    f.write(text)
    f.close()

class GetComPort():
    def __init__(self):
        self.root= Tk() # Create an instance of Tkinter frame
        self.root.geometry("300x100")
        #Initialize a Label to display the User Input
        label=Label(self.root, text="Enter Com Port")
        label.pack()
        #Create an Entry widget to accept User Input
        self.entry= Entry(self.root, width=40)
        self.entry.insert(0,readFile("comPort.txt"))
        self.entry.focus_set()
        self.entry.pack()
        #Create a Button to validate Entry Widget
        Button(self.root, text= "Okay",width= 20, command=self.accept).pack(pady=20)
        self.root.bind('<Return>', self.accept)
        self.root.mainloop()
        
    def accept(self, event=None):
        self.result = self.entry.get()
        print("using:", self.result)
        self.root.destroy()
        saveFile("comPort.txt", self.result)
        
    def get(self):
        return self.result
        
#============== run TKInter ==========

comPort = GetComPort().get()
        
master = Master(comPort)
master.root.mainloop()
