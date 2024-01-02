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
    experiment.addAgent(new Agent ("Constant0.9-OIV2", machine, new ConstantStepSizeValueEstimator(0.9, 2.0), new GreedyActionChooser()));
    experiment.addAgent(new Agent ("Constant0.5-OIV200", machine, new ConstantStepSizeValueEstimator(0.5, 200), new GreedyActionChooser()));
    experiment.addAgent(new Agent ("Constant0.9-OIV0", machine, new ConstantStepSizeValueEstimator(0.9, 0), new GreedyActionChooser()));
    experiment.run(1000, "../output/OptimisticInitialValues_Comparison.pdf");
}