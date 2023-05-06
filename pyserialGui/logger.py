import time

class Logger:
    def __init__(self):
        
        self.startTime = time.time()
        
        fileName = "LOGGER"+str(int(self.startTime))+".CSV"
        
        self.log = open(fileName, "a")
    
    def incoming(self, msg):
        for char in msg:
            if (char == "["):
                self.log.write("\n")
                self.log.write(str(time.time()-self.startTime)+",")
            elif (char == "]"):
                self.log.write(",")
            elif (char == "="):
                self.log.write(",")
            else:
                self.log.write(char)

    def close(self):
        self.log.close()
        

