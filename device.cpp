/**
 * @file main.cpp
 *
 * @brief A C++ program demonstrating the usage of the Stream and Device classes.
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include "gtest/gtest.h"

using namespace std;

int streamcounter; ///< Global variable to keep track of stream creation.

const int MIXER_OUTPUTS = 1;
const float POSSIBLE_ERROR = 0.01;

/**
 * @class Stream
 * @brief Represents a chemical stream with a name and mass flow.
 */
class Stream
{
private:
    double mass_flow; ///< The mass flow rate of the stream.
    string name;      ///< The name of the stream.

public:
    /**
     * @brief Constructor to create a Stream with a unique name.
     * @param s An integer used to generate a unique name for the stream.
     */
    Stream(int s){setName("s"+std::to_string(s));}

    /**
     * @brief Set the name of the stream.
     * @param s The new name for the stream.
     */
    void setName(string s){name=s;}

    /**
     * @brief Get the name of the stream.
     * @return The name of the stream.
     */
    string getName(){return name;}

    /**
     * @brief Set the mass flow rate of the stream.
     * @param m The new mass flow rate value.
     */
    void setMassFlow(double m){mass_flow=m;}

    /**
     * @brief Get the mass flow rate of the stream.
     * @return The mass flow rate of the stream.
     */
    double getMassFlow() const {return mass_flow;}

    /**
     * @brief Print information about the stream.
     */
    void print() { cout << "Stream " << getName() << " flow = " << getMassFlow() << endl; }
};

/**
 * @class Device
 * @brief Represents a device that manipulates chemical streams.
 */
class Device
{
protected:
    vector<shared_ptr<Stream>> inputs;  ///< Input streams connected to the device.
    vector<shared_ptr<Stream>> outputs; ///< Output streams produced by the device.
    int inputAmount;
    int outputAmount;
public:
    /**
     * @brief Add an input stream to the device.
     * @param s A shared_ptr to the input stream.
     */
    void addInput(shared_ptr<Stream> s){
      if(inputs.size() < inputAmount) inputs.push_back(s);
      else throw"INPUT STREAM LIMIT!";
    }
    /**
     * @brief Add an output stream to the device.
     * @param s A shared_ptr to the output stream.
     */
    void addOutput(shared_ptr<Stream> s){
      if(outputs.size() < outputAmount) outputs.push_back(s);
      else throw "OUTPUT STREAM LIMIT!";
    }

    /**
     * @brief Update the output streams of the device (to be implemented by derived classes).
     */
    virtual void updateOutputs() = 0;

    // Добавлены методы для доступа к потокам извне
    shared_ptr<Stream> getInput(int index) { return inputs.at(index); }
    shared_ptr<Stream> getOutput(int index) { return outputs.at(index); }
    int getInputCount() { return inputs.size(); }
    int getOutputCount() { return outputs.size(); }
};

class Mixer: public Device
{
    private:
      int _inputs_count = 0;
    public:
      Mixer(int inputs_count): Device() {
        _inputs_count = inputs_count;
        inputAmount = inputs_count;
        outputAmount = MIXER_OUTPUTS;
      }
      void addInput(shared_ptr<Stream> s) {
        if (inputs.size() == _inputs_count) {
          throw "Too much inputs"s;
        }
        inputs.push_back(s);
      }
      void addOutput(shared_ptr<Stream> s) {
        if (outputs.size() == MIXER_OUTPUTS) {
          throw "Too much outputs"s;
        }
        outputs.push_back(s);
      }
      void updateOutputs() override {
        double sum_mass_flow = 0;
        for (const auto& input_stream : inputs) {
          sum_mass_flow += input_stream -> getMassFlow();
        }

        if (outputs.empty()) {
          throw "Should set outputs before update"s;
        }

        double output_mass = sum_mass_flow / outputs.size();

        for (auto& output_stream : outputs) {
          output_stream -> setMassFlow(output_mass);
        }
      }
};

void shouldSetOutputsCorrectlyWithOneOutput() {
    streamcounter=0;
    Mixer d1 = Mixer(2);
    
    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    shared_ptr<Stream> s3(new Stream(++streamcounter));
    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);

    d1.addInput(s1);
    d1.addInput(s2);
    d1.addOutput(s3);

    d1.updateOutputs();

    if (abs(s3->getMassFlow() - 15) < POSSIBLE_ERROR) {
      cout << "Test 1 passed"s << endl;
    } else {
      cout << "Test 1 failed"s << endl;
    }
}

void shouldCorrectOutputs() {
    streamcounter=0;
    Mixer d1 = Mixer(2);
    
    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    shared_ptr<Stream> s3(new Stream(++streamcounter));
    shared_ptr<Stream> s4(new Stream(++streamcounter));
    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);

    d1.addInput(s1);
    d1.addInput(s2);
    d1.addOutput(s3);

    try {
      d1.addOutput(s4);
    } catch (const string ex) {
      if (ex == "Too much outputs"s) {
        cout << "Test 2 passed"s << endl;

        return;
      }
    }

    cout << "Test 2 failed"s << endl;
}

void shouldCorrectInputs() {
    streamcounter=0;
    Mixer d1 = Mixer(2);
    
    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    shared_ptr<Stream> s3(new Stream(++streamcounter));
    shared_ptr<Stream> s4(new Stream(++streamcounter));
    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);

    d1.addInput(s1);
    d1.addInput(s2);
    d1.addOutput(s3);

    try {
      d1.addInput(s4);
    } catch (const string ex) {
      if (ex == "Too much inputs"s) {
        cout << "Test 3 passed"s << endl;

        return;
      }
    }

    cout << "Test 3 failed"s << endl;
}

class Reactor : public Device{
public:
    Reactor(bool isDoubleReactor) {
        inputAmount = 1;
        if (isDoubleReactor) 
            outputAmount = 2;
        else 
            outputAmount = 1;
    }
    
    void updateOutputs() override{
        double inputMass = inputs.at(0) -> getMassFlow();
        for(int i = 0; i < outputAmount; i++){
            double outputLocal = inputMass * (1.0/outputAmount);
            outputs.at(i) -> setMassFlow(outputLocal);
        }
    }
};

void testTooManyOutputStreams(){
    streamcounter=0;
    
    Reactor dl(false);
    
    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    shared_ptr<Stream> s3(new Stream(++streamcounter));
    s1->setMassFlow(10.0);
    dl.addInput(s1);
    dl.addOutput(s2);
    try{
        dl.addOutput(s3);
    } catch(const char* ex){
         if (string(ex) == "OUTPUT STREAM LIMIT!")
            cout << "Test 1 passed" << endl;
        return;
    }
    
    cout << "Test 1 failed" << endl;
}


/**
* @class Divider
* @brief Устройство, разделяющее один вх поток на N вых потоков с равным массовым расходом.
*/
class Divider : public Device
{
public:
    /**
    * @brief Создание нового делителя.
    * @param outputs_count Число вых потоков.
    */
    Divider(int outputs_count);
     /**
    * @brief Обновляет массовый расход всех вых потоков.
    * @details Разделение вх массового расхода поровну между всеми вых.
    * @throw Выдает исключение при незаданных вх/вых.
    */
    void updateOutputs() override;
};

Divider::Divider(int outputs_count) {
    inputAmount = 1;
    outputAmount = outputs_count;
}

void Divider::updateOutputs() {
    if (inputs.empty() || outputs.empty()) {
        throw "Делитель должен иметь входные и выходные данные до обновления.";
    }
    double input_mass = inputs[0]->getMassFlow();
    double output_mass = input_mass / outputs.size();

    for (auto& output_stream : outputs) {
        output_stream->setMassFlow(output_mass);
    }
}

/**
 * @brief Тест: делитель правильно делит поток на 3 равных выхода
 */
void testDividerDividesFlowEqually() {
    std::cout << "DividerTest1: Разделение на выходы" << std::endl;
    streamcounter = 0;
    Divider d1(3);

    auto s_in = std::make_shared<Stream>(++streamcounter);
    auto s_out1 = std::make_shared<Stream>(++streamcounter);
    auto s_out2 = std::make_shared<Stream>(++streamcounter);
    auto s_out3 = std::make_shared<Stream>(++streamcounter);

    s_in->setMassFlow(12.0);
    d1.addInput(s_in);
    d1.addOutput(s_out1);
    d1.addOutput(s_out2);
    d1.addOutput(s_out3);

    d1.updateOutputs();

    if (std::abs(s_out1->getMassFlow() - 4.0) < POSSIBLE_ERROR &&
        std::abs(s_out2->getMassFlow() - 4.0) < POSSIBLE_ERROR &&
        std::abs(s_out3->getMassFlow() - 4.0) < POSSIBLE_ERROR) {
        std::cout << "Passed" << std::endl;
    } else {
        std::cout << "Failed" << std::endl;
    }
}

/**
 * @brief Тест: сумма выходных потоков = входному потоку
 */
void testDividerMassConservation() {
    std::cout << "DividerTest2: Cумма выходов = входу" << std::endl;
    streamcounter = 0;
    Divider d1(2);

    auto s_in = std::make_shared<Stream>(++streamcounter);
    auto s_out1 = std::make_shared<Stream>(++streamcounter);
    auto s_out2 = std::make_shared<Stream>(++streamcounter);

    s_in->setMassFlow(10.0);
    d1.addInput(s_in);
    d1.addOutput(s_out1);
    d1.addOutput(s_out2);

    d1.updateOutputs();

    double total_output = s_out1->getMassFlow() + s_out2->getMassFlow();
    if (std::abs(total_output - 10.0) < POSSIBLE_ERROR) {
        std::cout << "Passed" << std::endl;
    } else {
        std::cout << "Failed" << std::endl;
    }
}

/**
 * @brief Тест: поток не изменяется с 1 выходом
 */
void testDividerSingleOutput() {
    std::cout << "DividerTest3: Один выход" << std::endl;
    streamcounter = 0;
    Divider d1(1);

    auto s_in = std::make_shared<Stream>(++streamcounter);
    auto s_out = std::make_shared<Stream>(++streamcounter);

    s_in->setMassFlow(8.0);
    d1.addInput(s_in);
    d1.addOutput(s_out);

    d1.updateOutputs();

    if (std::abs(s_out->getMassFlow() - 8.0) < POSSIBLE_ERROR) {
        std::cout << "Passed" << std::endl;
    } else {
        std::cout << "Failed" << std::endl;
    }
}

/**
 * @brief Тест: исключение при отсутствии входного потока
 */
void testDividerThrowsWhenNoInput() {
    std::cout << "DividerTest4: Исключение при отсутствии входов" << std::endl;
    streamcounter = 0;
    Divider d1(2);
    auto s_out = std::make_shared<Stream>(++streamcounter);
    d1.addOutput(s_out);
    
    try {
        d1.updateOutputs();
        std::cout << "Failed" << std::endl;
    } catch (const char* e) {
        std::cout << "Passed" << std::endl;
    }
}

/**
 * @brief Тест: исключение при отсутствии выходных потоков
 */
void testDividerThrowsWhenNoOutputs() {
    std::cout << "DividerTest5: Исключение при отсутствии выходов" << std::endl;
    streamcounter = 0;
    Divider d1(2);
    auto s_in = std::make_shared<Stream>(++streamcounter);
    s_in->setMassFlow(10.0);
    d1.addInput(s_in);
    
    try {
        d1.updateOutputs();
        std::cout << "Failed" << std::endl;
    } catch (const char* e) {
        std::cout << "Passed" << std::endl;
    }
}

/**
 * @brief Тест: исключение при попытке добавить больше 1 входа
 */
void testDividerThrowsWhenTooManyInputs() {
    std::cout << "DividerTest6: Исключение при слишком большем кол-ве входов" << std::endl;
    streamcounter = 0;
    Divider d1(2);

    auto s_in1 = std::make_shared<Stream>(++streamcounter);
    auto s_in2 = std::make_shared<Stream>(++streamcounter);
    auto s_out = std::make_shared<Stream>(++streamcounter);

    d1.addInput(s_in1);
    
    try {
        d1.addInput(s_in2);
        std::cout << "Failed" << std::endl;
    } catch (const char* e) {
        std::cout << "Passed" << std::endl;
    }
}

/**
 * @brief Основная тестовая функция для Divider (отдельная тестовая функция)
 */
void runDividerTests() {
    testDividerDividesFlowEqually();      // Тест 1
    testDividerMassConservation();        // Тест 2  
    testDividerSingleOutput();            // Тест 3
    testDividerThrowsWhenNoInput();       // Тест 4
    testDividerThrowsWhenNoOutputs();     // Тест 5
    testDividerThrowsWhenTooManyInputs(); // Тест 6
    
}



void testTooManyInputStreams(){
    streamcounter=0;
    
    Reactor dl(false);
    
    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    shared_ptr<Stream> s3(new Stream(++streamcounter));
    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);
    dl.addInput(s1);
    try{
        dl.addInput(s3);
    } catch(const char* ex){
         if (string(ex) == "INPUT STREAM LIMIT!")
            cout << "Test 2 passed" << endl;
        return;
    }
    
    cout << "Test 2 failed"s << endl;
}

void testInputEqualOutput(){
    streamcounter=0;
    
    Reactor dl(true);
    
    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    shared_ptr<Stream> s3(new Stream(++streamcounter));
    s1->setMassFlow(10.0);
    dl.addInput(s1);
    dl.addOutput(s2);
    dl.addOutput(s3);
    
    dl.updateOutputs();
    
    // Используем публичные методы для доступа к данным
    double output1 = dl.getOutput(0)->getMassFlow();
    double output2 = dl.getOutput(1)->getMassFlow();
    double input = dl.getInput(0)->getMassFlow();
    
    if(abs((output1 + output2) - input) < POSSIBLE_ERROR)
        cout << "Test 3 passed" << endl;
    else
        cout << "Test 3 failed" << endl;
}

void tests(){
    testInputEqualOutput();
    testTooManyOutputStreams();
    testTooManyInputStreams();
    
    shouldSetOutputsCorrectlyWithOneOutput();
    shouldCorrectOutputs();
    shouldCorrectInputs();

     runDividerTests();
}

/**
 * @brief The entry point of the program.
 * @return 0 on successful execution.
 */
int main()
{
    streamcounter = 0;

    // Create streams
    shared_ptr<Stream> s1(new Stream(++streamcounter));
    shared_ptr<Stream> s2(new Stream(++streamcounter));
    shared_ptr<Stream> s3(new Stream(++streamcounter));

    // Set mass flows
    s1->setMassFlow(10.0);
    s2->setMassFlow(5.0);

    tests();

    return 0;
}
