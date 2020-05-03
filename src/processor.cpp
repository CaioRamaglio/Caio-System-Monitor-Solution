#include <iostream>
#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
    float utilization = (float)LinuxParser::ActiveJiffies()/(float)LinuxParser::Jiffies();
    return utilization;
}