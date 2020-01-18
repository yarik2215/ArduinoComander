/*TODO: сделать диспетчер задач, системный таймер и очередь в виде кольцевого буфера (продумать мьютексы)*/

/*TODO:если на проходе по списку вызывается функция у задачи, которая помещает задачу в конец списка с временем 0, то задача будет все время туда помещатся и не выходить из цикла,
нужно добавить флаг который бы показывал, что данная задача уже выполнялась в этом цикле update*/

#pragma once

#include <Arduino.h>
#include <stdint.h>

// #define avr
// #define esp

// период таймера для работы с частотой в 1кГц
// #define FREQ_TIMER 16000UL


struct Task{
    Task(void(*f)()) : func(f) {}
    void (*func)() = nullptr;
    uint64_t timer = 0;
    bool mutex = false;
    Task *nextTask = nullptr;
    bool fExecuted = false;
};


class Commander{
public:
    Commander() = default;
    ~Commander() = default;
    void begin();
    void check(); /*проверяет очередь и убирает из нее задачи после их выполнения*/
    bool moveToQueue(Task* task, uint64_t time); 
    void updateTimer();

private:
    Task *head = nullptr;
};

/*
void Commander::begin()
{
    cli();
    TCCR1B = 0b001;  //clock div1
    TCNT1 = 0;
    TCCR1A = 0;
    OCR1A = FREQ_TIMER;
    TIMSK1 |= (1 << OCIE1A);
    SREG |= (1<<SREG_I);
    sei();
}
*/

void Commander::check()
{
    if(head == nullptr)
        return;         //пустой список

    Task *prevTask = nullptr;
    Task *curTask = head;
    while(curTask != nullptr) {
        //выполнение задачи по истечению таймера и если функция не заблокирована
        Task *nextTask = curTask->nextTask;

        if(!curTask->mutex && !curTask->fExecuted && curTask->timer <= millis()) {
            if(prevTask == nullptr) {
                head = curTask->nextTask;
            }
            else {
                prevTask->nextTask = curTask->nextTask;
                //curTask = prevTask;
            }
            curTask->func();
            curTask->fExecuted = true;
        }
        else {
            prevTask = curTask;
            curTask->fExecuted = false;
        }
        curTask = nextTask;
    }
}

/*
    Move task to queue that will emit after time_ms
*/
bool Commander::moveToQueue(Task* task, uint64_t time_ms)
{
    cli();
    task->timer = millis() + time_ms;
    task->nextTask = nullptr;
    sei();
    
    // Serial.println("Move task");
    if(head == nullptr) {
        head = task;
        // Serial.println("Move to head");
        return;
    }
    Task *curTask = head;
    while (curTask->nextTask != nullptr)
    {
        curTask = curTask->nextTask;
    }
    curTask->nextTask = task;
    // Serial.println("Move to end");
} 


void Commander::updateTimer()
{
    Task *curTask = head;
    while(curTask != nullptr){
        if(curTask->timer != 0) {
            curTask->timer--;
        }
        curTask = curTask->nextTask;
    }
}


Commander commander;

/*
ISR(TIMER1_COMPA_vect)
{
  TCNT1 = 0;
  sei();
  commander.updateTimer();
}
*/