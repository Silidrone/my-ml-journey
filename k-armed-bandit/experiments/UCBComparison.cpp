//
// Created by silidrone on 30.12.2023.
//
#include "../headers/GaussianMAB.h"
#include "../headers/ComparisonExperiment.h"
#include "../headers/SampleAvgValueEstimator.h"
#include "../headers/EpsilonGreedyActionChooser.h"
#include "../headers/ConstantStepSizeValueEstimator.h"
#include "../headers/UCBActionChooser.h"

int main() {
    MAB *machine = new GaussianMAB(10);
    ComparisonExperiment experiment(machine, 2000);
    experiment.addAgent(new Agent ("Constant0.9-Greedy", machine, new ConstantStepSizeValueEstimator(0.9), new GreedyActionChooser()));
    experiment.addAgent(new Agent ("SampleAvg-UCB5", machine, new SampleAvgValueEstimator(0.0), new UCBActionChooser(5.0)));
    experiment.run(10000, "../output/UpperConfidenceBounds_Comparison.pdf");
}