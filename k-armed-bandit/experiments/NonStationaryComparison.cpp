//
// Created by silidrone on 30.12.2023.
//
#include "../headers/GaussianMAB.h"
#include "../headers/ComparisonExperiment.h"
#include "../headers/SampleAvgValueEstimator.h"
#include "../headers/EpsilonGreedyActionChooser.h"
#include "../headers/ConstantStepSizeValueEstimator.h"
#include "../headers/UCBActionChooser.h"
#include "../headers/NonStationaryMABDecorator.h"

int main() {
    MAB *machine = new NonStationaryMABDecorator(new GaussianMAB(10), 500);
    ComparisonExperiment experiment(machine, 2000);
    experiment.addAgent(new Agent ("Constant0.9-Epsilon0.1", machine, new ConstantStepSizeValueEstimator(0.9), new EpsilonGreedyActionChooser(0.1)));
    experiment.addAgent(new Agent ("SampleAvg-Epsilon0.1", machine, new SampleAvgValueEstimator(0.0), new EpsilonGreedyActionChooser(0.1)));
    experiment.addAgent(new Agent ("SampleAvg-UCB5", machine, new SampleAvgValueEstimator(0.0), new UCBActionChooser(5)));
    experiment.run(10000, "../output/NonStationary_Comparison.pdf");
}