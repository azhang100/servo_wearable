
from tkinter import *

#============ Variables TODO ==============

class Graph:
    GRAPH_WIDTH = 600
    LEGEND_WIDTH = 50
    GRAPH_BUFFER = 50
    GRAPH_HEIGHT = 250+GRAPH_BUFFER*2
    GRAPH_HEIGHT_MIN = GRAPH_HEIGHT-GRAPH_BUFFER
    GRAPH_HEIGHT_MAX = GRAPH_BUFFER
    
    UDPATE_PERIOD = 1000
    
    COLORS = ("red", "blue", "green", "orange", "purple", "brown", "pink", "turquoise",
              "red", "blue", "green", "orange", "purple", "brown", "pink", "turquoise",
              "red", "blue", "green", "orange", "purple", "brown", "pink", "turquoise")

    def __init__(self, master):
        self.master = master
        self.root = master.root
        self.root.after(100, self.loop)
        
        self.graphFrame = Frame(self.root)
        
        self.graphCanvas = Canvas(self.graphFrame, bg="white", 
                                  height=Graph.GRAPH_HEIGHT, width=Graph.GRAPH_WIDTH)
        self.graphCanvas.grid(row=0,column=1)
        self.graphXAxis = Canvas(self.graphFrame, bg="white", 
                                  height=Graph.LEGEND_WIDTH, width=Graph.GRAPH_WIDTH)
        self.graphXAxis.grid(row=1,column=1)
        self.legendFrame = Frame(self.graphFrame)
        self.legendFrame.grid(row=0,column=0)
        Button(self.graphFrame, text="Clear\nVariables", command=self.clearVariables).grid(row=1, column=0)
        
        # Variables
        self.graphVariableKeys = []
        self.legendCanvases = []
        
        # Data / x Axis
        self.dots = []
        for xPos in range(Graph.GRAPH_WIDTH):
            self.dots.append([])
        self.xAxisIndex = 0
        self.drawXAxis()
        
        self.graphFrame.pack(side=TOP)
        self.toggleVariable("egco2")
        self.toggleVariable("sweep")
        self.toggleVariable("tegco2")
    
    def drawXAxis(self):
        STEP = 60;
        self.graphXAxis.create_text(Graph.GRAPH_WIDTH/2, Graph.LEGEND_WIDTH/2, anchor="n", 
                                    text= "Time (s)", fill="black", font=('Helvetica 12 bold'))
        
        # zero tick
        xPos = 0
        self.graphXAxis.create_line(xPos+3, 10, xPos+3, 0)
        self.graphXAxis.create_text(xPos+3, 10, anchor="nw", fill="black",
                                    text=str(xPos*Graph.UDPATE_PERIOD//1000))
                                    
        # most ticks
        while xPos<Graph.GRAPH_WIDTH-STEP:
            xPos += STEP
            self.graphXAxis.create_line(xPos, 10, xPos, 0)
            self.graphXAxis.create_text(xPos, 10, anchor="n", fill="black",
                                        text=str(xPos*Graph.UDPATE_PERIOD//1000))
        # last tick
        xPos += STEP
        self.graphXAxis.create_line(xPos, 10, xPos, 0)
        self.graphXAxis.create_text(xPos, 10, anchor="ne", fill="black",
                                    text=str(xPos*Graph.UDPATE_PERIOD//1000))
    
    def toggleVariable(self, key):
        if key not in self.graphVariableKeys:
            index = len(self.graphVariableKeys)
            # add to variables
            self.graphVariableKeys.append(key)
            # add legend
            variable = self.master.variableManager.variables[key]
            
            # draw legend
            legendCanvas = Canvas(self.legendFrame, bg=Graph.COLORS[index], 
                                  height=Graph.GRAPH_HEIGHT, width=Graph.LEGEND_WIDTH)
            self.drawLegend(legendCanvas, variable)
            
                                     
            legendCanvas.pack(side=LEFT)
            self.legendCanvases.append(legendCanvas)
    
    def drawLegend(self, legendCanvas, variable):
        legendCanvas.create_text(Graph.LEGEND_WIDTH/2, Graph.GRAPH_HEIGHT/2, anchor="s", angle=90,
                                 text=variable.name, fill="white", font=('Helvetica 12 bold'))
                                 
        # math
        STEP=50
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
        for legend in self.legendCanvases:
            legend.pack_forget()
            legend.destroy()
        self.graphVariableKeys = []
        self.legendCanvases = []
    
    def loop(self):
        self.xAxisIndex = (self.xAxisIndex+1)%Graph.GRAPH_WIDTH
        
        for dot in self.dots[self.xAxisIndex]:
            self.graphCanvas.delete(dot)
        
        colorIndex = 0
        for key in self.graphVariableKeys:
            variable = self.master.variableManager.variables[key]
            
            if (isinstance(variable.val, int)): # else variable.val = "N/A"
                yPos = mapRange(variable.val, variable.minDispVal, variable.maxDispVal, 
                                Graph.GRAPH_HEIGHT_MIN, Graph.GRAPH_HEIGHT_MAX)
                
                dot = self.graphCanvas.create_rectangle(self.xAxisIndex-1, yPos-1, 
                                                        self.xAxisIndex+1, yPos+1, 
                                                        fill=Graph.COLORS[colorIndex], width=0)
                                                        # not border, line, edge
                self.dots[self.xAxisIndex].append(dot)
            colorIndex += 1
        
        self.root.after(Graph.UDPATE_PERIOD, self.loop) # update graph again soon
        
def mapRange(x, in_min, in_max, out_min, out_max):
    # could fix the snakecase but too lazy right now
    return (x - in_min) * (out_max - out_min) // (in_max - in_min) + out_min