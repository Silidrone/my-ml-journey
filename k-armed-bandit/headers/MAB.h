//
// Created by silidrone on 29.12.2023.
//
#ifndef MAB_H
#define MAB_H

class MAB {
protected:
    int m_arm_count;
    std::vector<double> m_qstar_values;

    friend class NonStationaryMABDecorator;
public:
    explicit MAB(int arm_count) : m_arm_count(arm_count) {}
    virtual ~MAB() = default;

    virtual double roll(int action, int timestep) = 0;
    virtual void reset() = 0;
    virtual void printQStar() {
        for (int i = 0; i < m_qstar_values.size(); i++) {
            std::cout << "[" << i << "]: " << m_qstar_values[i] << std::endl;
        }
    }
    int getArmCount() const { return m_arm_count; }
};

#endif //MAB_H
