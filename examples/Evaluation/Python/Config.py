class Config:
    def __init__(self):    
        self.seed   = 20081976
        self.dim    =     2         
        self.nmin   =    16         # 2^4
        self.nmax   =  4096         # 2^15 + 1
        self.factor =     16         # 
        self.m      =    16         # Number of pointsets to
                                    # generate for each size
        self.nInt   = self.nmax * 2 # N for integral computation
                                    # Make sure it is way above nmax 
        self.mInt   =  4096         # Number of integrands