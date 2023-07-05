import pyutk

class Evaluator:
    class Eval:
        def __init__(self) -> None:
            self.N = 0
            self.d = 0

            self.gl2 = 0.0
            self.gauss = 0.0
            self.heavi = 0.0
        
        def __add__(self, other):
            rslt = Evaluator.Eval()
            rslt.N = self.N
            rslt.d = self.d
            rslt.gl2   = self.gl2   + other.gl2
            rslt.gauss = self.gauss + other.gauss
            rslt.heavi = self.heavi + other.heavi

            return rslt
        
        def __truediv__(self, N):
            rslt = Evaluator.Eval()
            rslt.N = self.N
            rslt.d = self.d
            rslt.gl2   = self.gl2   / N
            rslt.gauss = self.gauss / N
            rslt.heavi = self.heavi / N

            return rslt
        
    def __init__(self, config):
        self.gauss = pyutk.IntegrationTest()
        self.heavi = pyutk.IntegrationTest()

        samples = pyutk.Sobol(config.dim, 32).sample(config.nInt)
        
        self.gauss.BuildGaussianDatabase ("tmp_gauss.txt", config.dim, config.mInt, config.seed, {}, samples)
        self.heavi.BuildHeavisideDatabase("tmp_heavi.txt", config.dim, config.mInt, config.seed, {}, samples)

    def EvalPointset(self, pts):
        gl2 = pyutk.GL2Discrepancy().compute(pts)
        gauss = self.gauss.compute(pts).mean
        heavi = self.heavi.compute(pts).mean

        eval = Evaluator.Eval()
        eval.N = pts.shape[0]
        eval.d = pts.shape[1]
        eval.gl2 = gl2
        eval.gauss = gauss
        eval.heavi = heavi

        return eval
