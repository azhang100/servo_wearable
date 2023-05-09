"""
README: This is not a standalone executable file
it is called in variables.py

    with open('variableExec.py', 'r') as file:
        data = file.read()
    globals = {"self":self}
    exec(data, globals)

Make sure all code in this file follows the following format,
    self.addVariable(name, rw, minDispVal, maxDispVal)
    
If there is funny code in this file, it can be used maliciously.
Yea, we'll probably have to properly integrate it for release.
"""

# sensor module    
self.addVariable("egco2","R")
self.addVariable("pressureChamber","R")
self.addVariable("humidity","R")
self.addVariable("temp","R")
self.addVariable("tTemp","RW")
self.addVariable("heaterPWM","R")
self.addVariable("pumpPWM","R")

# both systems
self.addVariable("tegco2","RW")

# servoWearable only
self.addVariable("suggestSweep", "R", 0, 10000)
self.addVariable("tSweep", "R", 0, 10000)
self.addVariable("sweep", "R", 0, 10000)
self.addVariable("flow", "R", 0, 10)
self.addVariable("sweepP", "RW", 0, 10000)
self.addVariable("sweepI", "RW", 0, 10000)
self.addVariable("sweepD", "RW", 0, 10000)
self.addVariable("pressureInput", "R")
self.addVariable("systemVersion", "R")

# servoVA only
self.addVariable("sweepO2", "R", 0, 10000)
self.addVariable("sweepCO2", "R", 0, 10000)
self.addVariable("sweepN2", "R", 0, 10000)
self.addVariable("tSweepO2", "R", 0, 10000)
self.addVariable("tSweepCO2", "R", 0, 10000)
self.addVariable("tSweepN2", "R", 0, 10000)

# misc
self.addVariable("systemVersion", "R")
