
from tkinter import *

#============ Variables TODO ==============

class Graph:
    GRAPH_WIDTH = 600
    LEGEND_WIDTH = 40
    XAXIS_HEIGHT = 40
    GRAPH_BUFFER = 50
    GRAPH_HEIGHT = 200+GRAPH_BUFFER*2
    GRAPH_HEIGHT_MIN = GRAPH_HEIGHT-GRAPH_BUFFER
    GRAPH_HEIGHT_MAX = GRAPH_BUFFER
    
    COLORS = ("red", "blue", "green", "orange", "purple", "brown", "pink", "turquoise")

    def __init__(self, master, frame, updatePeriod, xStep):
        self.master = master
        self.root = master.root
        self.root.after(100, self.loop)
        
        self.graphFrame = frame
        
        self.graphCanvas = Canvas(self.graphFrame, bg="white", 
                                  height=Graph.GRAPH_HEIGHT, width=Graph.GRAPH_WIDTH)
        self.graphCanvas.grid(row=0,column=1)
        self.graphXAxis = Canvas(self.graphFrame, bg="white", 
                                  height=Graph.XAXIS_HEIGHT, width=Graph.GRAPH_WIDTH)
        self.graphXAxis.grid(row=1,column=1)
        self.legendFrame = Frame(self.graphFrame)
        self.legendFrame.grid(row=0,column=0)
        Button(self.graphFrame, text="Clear\nVariables", command=self.clearVariables).grid(row=1, column=0)
        
        # constants
        self.updatePeriod = updatePeriod
        self.xStep = xStep
        
        # Variables
        self.graphVariableKeys = []
        self.legendCanvases = []
        for i in range(4):
            self.graphVariableKeys.append(None)
            canvas = Canvas(self.legendFrame, # bg=Graph.COLORS[i], 
                            height=Graph.GRAPH_HEIGHT, width=Graph.LEGEND_WIDTH)
            self.legendCanvases.append(canvas)
            canvas.pack(side=LEFT)
        
        # Data / x Axis
        self.dots = []
        for xPos in range(Graph.GRAPH_WIDTH):
            self.dots.append([])
        self.xAxisIndex = 0
        self.drawXAxis()
        
        self.toggleVariable("egco2")
        self.toggleVariable("sweep")
        self.toggleVariable("tegco2")
    
    def drawXAxis(self):
        if (self.updatePeriod < 1000):
            xLegend = "Time (s)"
            updatePeriodDiv = 1
        else:
            xLegend = "Time (m)"
            updatePeriodDiv = 60
        
        self.graphXAxis.create_text(Graph.GRAPH_WIDTH/2, 25, anchor="n", 
                                    text= xLegend, fill="black", font=('Helvetica 10 bold'))
        
        # zero tick
        xPos = 0
        self.graphXAxis.create_line(xPos+2, 10, xPos+2, 0)
        self.graphXAxis.create_text(xPos+2, 10, anchor="nw", fill="black",
                                    text=str(xPos*self.updatePeriod//updatePeriodDiv//1000))
                                    
        # most ticks
        while xPos<Graph.GRAPH_WIDTH-self.xStep:
            xPos += self.xStep
            self.graphXAxis.create_line(xPos, 10, xPos, 0)
            self.graphXAxis.create_text(xPos, 10, anchor="n", fill="black",
                                        text=str(xPos*self.updatePeriod//updatePeriodDiv//1000))
        # last tick
        xPos += self.xStep
        self.graphXAxis.create_line(xPos, 10, xPos, 0)
        self.graphXAxis.create_text(xPos, 10, anchor="ne", fill="black",
                                    text=str(xPos*self.updatePeriod//updatePeriodDiv//1000))
    
    def toggleVariable(self, key):
        print("toggleVariable('"+str(key)+"')")
        if key in self.graphVariableKeys:
            index = self.graphVariableKeys.index(key)
            print("removing key at index", index)
            self.graphVariableKeys[index] = None
            self.legendCanvases[index].delete("all")
            self.legendCanvases[index].configure(bg="white")
        else:
            index = self.graphVariableKeys.index(None)
            print("adding key at index", index)
            # add to variables
            self.graphVariableKeys[index] = key
            # add legend
            variable = self.master.variableManager.variables[key]
            self.legendCanvases[index].configure(bg=Graph.COLORS[index])
            self.drawLegend(self.legendCanvases[index], variable)
    
    def drawLegend(self, legendCanvas, variable):
        legendCanvas.create_text(18, Graph.GRAPH_HEIGHT, anchor="sw", angle=90,
                                 text=variable.name, fill="white", font=('Helvetica 10 bold'))
                                 
        # math
        STEP=20
        pixelVal = (variable.maxDispVal-variable.minDispVal)/(Graph.GRAPH_HEIGHT_MIN-Graph.GRAPH_HEIGHT_MAX)
                 
        # zero tick=
        
        yPixelPos = 0 #reminder that coordinates in TKInter are reversed
        while yPixelPos <= Graph.GRAPH_HEIGHT_MIN-Graph.GRAPH_HEIGHT_MAX:
            yPos = Graph.GRAPH_HEIGHT_MIN - yPixelPos
            val = int(variable.minDispVal + yPixelPos * pixelVal)
            legendCanvas.create_line(Graph.LEGEND_WIDTH-10, yPos, Graph.LEGEND_WIDTH, yPos, fill="white")
            legendCanvas.create_text(Graph.LEGEND_WIDTH-10, yPos, anchor="e", text=str(val), fill="white")
            
            yPixelPos += STEP
    
    def clearVariables(self):
        for key in self.graphVariableKeys:
            self.toggleVariable(key)
    
    def loop(self):
        self.xAxisIndex = (self.xAxisIndex+1)%Graph.GRAPH_WIDTH
        
        for dot in self.dots[self.xAxisIndex]:
            self.graphCanvas.delete(dot)
        
        colorIndex = 0
        for key in self.graphVariableKeys:
            if (key==None): break        
            variable = self.master.variableManager.variables[key]
            if (isinstance(variable.val, int) or isinstance(variable.val, float)): # else variable.val = "N/A"
                yPos = mapRange(variable.val, variable.minDispVal, variable.maxDispVal, 
                                Graph.GRAPH_HEIGHT_MIN, Graph.GRAPH_HEIGHT_MAX)
                
                dot = self.graphCanvas.create_rectangle(self.xAxisIndex-1, yPos-1, 
                                                        self.xAxisIndex+1, yPos+1, 
                                                        fill=Graph.COLORS[colorIndex], width=0)
                                                        # not border, line, edge
                self.dots[self.xAxisIndex].append(dot)
            colorIndex += 1
        
        self.root.after(self.updatePeriod, self.loop) # update graph again soon
        
def mapRange(x, in_min, in_max, out_min, out_max):
    # could fix the snakecase but too lazy right now
    return (x - in_min) * (out_max - out_min) // (in_max - in_min) + out_min