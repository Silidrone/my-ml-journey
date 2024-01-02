//
// Created by silidrone on 30.12.2023.
//
#include "../headers/GaussianMAB.h"
#include "../headers/ComparisonExperiment.h"
#include "../headers/SampleAvgValueEstimator.h"
#include "../headers/EpsilonGreedyActionChooser.h"

int main() {
    MAB *machine = new GaussianMAB(10);
    ComparisonExperiment experiment(machine, 2000);
    experiment.addAgent(new Agent ("SampleAvg-Epsilon0.1", machine, new SampleAvgValueEstimator(), new EpsilonGreedyActionChooser(0.1)));
    experiment.addAgent(new Agent ("SampleAvg-Greedy", machine, new SampleAvgValueEstimator(), new GreedyActionChooser()));
    experiment.addAgent(new Agent ("SampleAvg-Random", machine, new SampleAvgValueEstimator(), new RandomActionChooser()));
    experiment.run(1000, "../output/SampleAverage_AC_Comparison.pdf");
}