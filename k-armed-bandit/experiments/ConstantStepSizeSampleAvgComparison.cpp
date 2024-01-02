//
// Created by silidrone on 30.12.2023.
//
#include "../headers/GaussianMAB.h"
#include "../headers/ComparisonExperiment.h"
#include "../headers/SampleAvgValueEstimator.h"
#include "../headers/EpsilonGreedyActionChooser.h"
#include "../headers/ConstantStepSizeValueEstimator.h"

int main() {
    MAB *machine = new GaussianMAB(10);
    ComparisonExperiment experiment(machine, 2000);
    experiment.addAgent(new Agent ("Constant0.5-Epsilon0.1", machine, new ConstantStepSizeValueEstimator(0.9), new EpsilonGreedyActionChooser(0.1)));
    experiment.addAgent(new Agent ("Constant0.9-Epsilon0.1", machine, new ConstantStepSizeValueEstimator(0.5), new EpsilonGreedyActionChooser(0.1)));
    experiment.addAgent(new Agent ("SampleAvg0.9-Epsilon0.1", machine, new SampleAvgValueEstimator(0.9), new EpsilonGreedyActionChooser(0.1)));
    experiment.run(1000, "../output/ConstantStepSize_SampleAvg_Comparison.pdf");
}