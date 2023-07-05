import pyutk
import pandas as pd

from tqdm import tqdm

from Config import Config
from Eval import Evaluator

def EvaluatePointets(config: Config, evaluator: Evaluator, samplers: dict):
    rslts = {"name": [], "N": [], "dim": [], "metric": [], "value": []}
    
    Ns = [config.nmin]
    while Ns[-1] < config.nmax:
        Ns.append(Ns[-1] * config.factor)

    for name, sampler in tqdm(samplers.items()):
        for n in tqdm(Ns, leave=False):
            eval = Evaluator.Eval()
            eval.n = n
            eval.d = config.dim

            for _ in tqdm(range(config.m), leave=False):
                pts = sampler.sample(n)
                eval = eval + evaluator.EvalPointset(pts)
            
            eval = eval / config.m

            for mname, mvalue in zip(["gl2", "gauss", "heavi"], [eval.gl2, eval.gauss, eval.heavi]):
                rslts["name"].append(name)
                rslts["N"].append(n)
                rslts["dim"].append(config.dim)
                rslts["metric"].append(mname)
                rslts["value"].append(mvalue)
    
    return rslts

if __name__ == "__main__":
    if False:
        samplers = {
            # Classical samplers
            "WN":      pyutk.Whitenoise(2),
            "Strat":   pyutk.Stratified(2), 
            "Grid":    pyutk.RegularGrid(2),
            "HexGrid": pyutk.HexGrid(),

            # Blue noise
            "Poisson-to":   pyutk.DartThrowing(2, True), 
            "Poisson-to":   pyutk.DartThrowing(2, True, False),
            "Poisson-fast": pyutk.FastPoisson(),
            "Poisson-proj": pyutk.ProjectiveBlueNoise(2), 
            "Step":         pyutk.Step(), 
            "SinglePeak":   pyutk.SinglePeak(), 
            "Fattal":       pyutk.Fattal(),
            "ForcedRandom": pyutk.ForcedRandom(),

            # Low discrepancy
            "Hammersley":   pyutk.Hammersley(), 
            "Halton":       pyutk.Halton(2), 
            "LDBN":         pyutk.LDBN(), 
            "Niederreiter": pyutk.Niederreiter(2), 
            "Faure":        pyutk.Faure(2),
            "PMJ":          pyutk.PMJ(), 
            "Sobol":        pyutk.Sobol(2), 
            "Owen32":       pyutk.Sobol(2, 32),

            # Lattices
            "R1": pyutk.Rank1(), 
            "R1-fibo": pyutk.Rank1(False, True), 
            "Korobov": pyutk.Korobov(2), 
            "Kron":    pyutk.Kronecker(2),

            # Tiles
            "ART":     pyutk.ART(), 
            "Penrose": pyutk.Penrose(),

            # Misc
            "AAPatterns": pyutk.AAPatterns(), 
            "CMJ":        pyutk.CMJ(), 
            "NRooks":     pyutk.NRooks(2)
        }
    else:
        samplers = {
            "WN":      pyutk.Whitenoise(2),
            "Strat":   pyutk.Stratified(2), 
            # "Grid":    pyutk.RegularGrid(2),
            # "R1":      pyutk.Rank1(), 
            # "Owen32":  pyutk.Sobol(2, 32),
        }

    config = Config()
    evaluator = Evaluator(config)

    rslts = EvaluatePointets(config, evaluator, samplers)
    df = pd.DataFrame(rslts)
    df.to_csv("all.csv", index=False)