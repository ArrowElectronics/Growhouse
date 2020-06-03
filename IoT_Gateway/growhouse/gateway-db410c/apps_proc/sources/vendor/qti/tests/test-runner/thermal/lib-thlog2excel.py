#!/usr/bin/python
#Copyright (c) 2018, The Linux Foundation. All rights reserved.
#
#Redistribution and use in source and binary forms, with or without
#modification, are permitted provided that the following conditions are
#met:
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above
#      copyright notice, this list of conditions and the following
#      disclaimer in the documentation and/or other materials provided
#      with the distribution.
#    * Neither the name of The Linux Foundation nor the names of its
#      contributors may be used to endorse or promote products derived
#      from this software without specific prior written permission.
#
#THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
#WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
#MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
#ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
#BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
#BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
#OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
#IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Library responsible for converting the result of thermal test to excel file
#
# Pre requisites:
# - Python 2.7
# -- xlsxwriter (Python module)
# -- numpy (Python module)
#
# How to use:
#    $ python2 lib-thlog2excel.py <ThermalLog-File> <DhrystoneLog-File>
#

import sys
import numpy as np
import xlsxwriter

# global variables
global worksheet
global workbook
global worksheet_thermal
global worksheet_dhrystone

def main():
    global worksheet
    global workbook

    # get thermal and dhrystone log filename
    thermal_logfile = sys.argv[1]
    dhrystone_logfile = sys.argv[2]

    # create xlsxwriter workbook object
    workbook = xlsxwriter.Workbook("ThermalTest-ThermalZone-Results.xlsx")

    # create sheets
    create_thermal_sheet(thermal_logfile)
    create_dhrystone_sheet(dhrystone_logfile)

    # close workbook object
    workbook.close()

def create_thermal_sheet(thermal_logfile):
    global worksheet_thermal

    # default row and column position
    row_thermalsheet    = 1
    column_thermalsheet = 0

    # create thermal sheet
    worksheet_thermal = workbook.add_worksheet("Thermal")

    # convert logfile to array
    log_table = np.genfromtxt((conv(x) for x in open(thermal_logfile)), names=True, dtype=None)

    worksheet_thermal.write(row_thermalsheet - 1, column_thermalsheet,     	 'Time')
    worksheet_thermal.write(row_thermalsheet - 1, column_thermalsheet + 1,   'Zone0Temp')
    worksheet_thermal.write(row_thermalsheet - 1, column_thermalsheet + 2,   'Zone1Temp')
    worksheet_thermal.write(row_thermalsheet - 1, column_thermalsheet + 3,   'CPU0Freq')
    worksheet_thermal.write(row_thermalsheet - 1, column_thermalsheet + 4,   'CPU1Freq')
    worksheet_thermal.write(row_thermalsheet - 1, column_thermalsheet + 5,   'CPU2Freq')
    worksheet_thermal.write(row_thermalsheet - 1, column_thermalsheet + 6,   'CPU3Freq')

    # filling in thermal test logs to worksheet
    for Time,Zone0Temp,Zone1Temp,CPU0Freq,CPU1Freq,CPU2Freq,CPU3Freq in (log_table):
        worksheet_thermal.write(row_thermalsheet, column_thermalsheet,     Time)
        worksheet_thermal.write(row_thermalsheet, column_thermalsheet + 1, Zone0Temp)
        worksheet_thermal.write(row_thermalsheet, column_thermalsheet + 2, Zone1Temp)
        worksheet_thermal.write(row_thermalsheet, column_thermalsheet + 3, CPU0Freq)
        worksheet_thermal.write(row_thermalsheet, column_thermalsheet + 4, CPU1Freq)
        worksheet_thermal.write(row_thermalsheet, column_thermalsheet + 5, CPU2Freq)
        worksheet_thermal.write(row_thermalsheet, column_thermalsheet + 6, CPU3Freq)
        row_thermalsheet += 1

    create_temperature_chart(row_thermalsheet)      # create temperature chart
    create_cpufrequency_chart(row_thermalsheet)     # create cpu frequency chart

def create_temperature_chart(last_line):

    # create a new chart object
    chart = workbook.add_chart({'type': 'line'})

    # configure the Zone0Temp serie
    chart.add_series({
        'name':       '=Thermal!$B$1',
        'categories': '=Thermal!$A$3:$A$' + str(last_line),
        'values':     '=Thermal!$B$3:$B$'+ str(last_line),
        'line':       {'width': 2},
    })

    # configure the Zone1Temp serie
    chart.add_series({
        'name':       '=Thermal!$C$1',
        'categories': '=Thermal!$A$3:$A$' + str(last_line),
        'values':     '=Thermal!$C$3:$C$' + str(last_line),
        'line':       {'width': 2},
    })

    # configure chart
    chart.set_title ({'name': 'Temperature Tests'})     # default chart title
    chart.set_x_axis({'name': 'Time',                   # default x axis name
        'minor_gridlines': {'visible': True}            # default minor_gridlines
    })

    chart.set_y_axis({'name': 'Temperature',            # default y axis name
                        'min': 0,                       # default minimum value for axis range
                        'max': 80000,                   # default maximum value for axis range
    })
    chart.set_size({'width': 1050, 'height': 400})      # default chart dimensions
    chart.set_style(10)                                 # default chart style

    # insert the chart into the worksheet
    worksheet_thermal.insert_chart('I3', chart, {'x_offset': 25, 'y_offset': 10})

def create_cpufrequency_chart(last_line):

    # create a new chart object
    chart = workbook.add_chart({'type': 'line'})

    # configure the CPU0Freq serie
    chart.add_series({
        'name':       '=Thermal!$D$1',
        'categories': '=Thermal!$A$3:$A$' + str(last_line),
        'values':     '=Thermal!$D$3:$D$'+ str(last_line),
    })
    # configure the CPU2Freq serie
    chart.add_series({
        'name':       '=Thermal!$E$1',
        'categories': '=Thermal!$A$3:$A$' + str(last_line),
        'values':     '=Thermal!$E$3:$E$' + str(last_line),
    })
    # configure the CPU3Freq serie
    chart.add_series({
        'name':       '=Thermal!$F$1',
        'categories': '=Thermal!$A$3:$A$' + str(last_line),
        'values':     '=Thermal!$F$3:$F$' + str(last_line),
    })
    # Configure the CPU0Freq1 serie
    chart.add_series({
        'name':       '=Thermal!$G$1',
        'categories': '=Thermal!$A$3:$A$' + str(last_line),
        'values':     '=Thermal!$G$3:$G$' + str(last_line),
    })

    # configure chart
    chart.set_title ({'name': 'CPU Frequency'})     # default chart title
    chart.set_x_axis({'name': 'Time',                   # default x axis name
        'minor_gridlines': {'visible': True}            # default minor_gridlines
    })
    chart.set_y_axis({'name': 'CPU Frequency'})         # default x axis name
    chart.set_size({'width': 1050, 'height': 400})      # default chart dimensions
    chart.set_style(10)                                 # default chart style

    # insert the chart into the worksheet
    worksheet_thermal.insert_chart('I26', chart, {'x_offset': 25, 'y_offset': 10})

def create_dhrystone_sheet(dhrystone_logfile):

    global worksheet_dhrystone

    # default row and column position
    row_dhrysheet    = 1
    column_dhrysheet = 0

    # create thermal sheet
    worksheet_dhrystone = workbook.add_worksheet("Dhrystone")

    # convert logfile to array
    log_table = np.genfromtxt((conv(x) for x in open(dhrystone_logfile)), names=True, dtype=None)

    worksheet_dhrystone.write(row_dhrysheet - 1, column_dhrysheet,     	   'Start')
    worksheet_dhrystone.write(row_dhrysheet - 1, column_dhrysheet + 1,     'End')
    worksheet_dhrystone.write(row_dhrysheet - 1, column_dhrysheet + 2,     'DhrystoneScore1')
    worksheet_dhrystone.write(row_dhrysheet - 1, column_dhrysheet + 3,     'DhrystoneScore2')
    worksheet_dhrystone.write(row_dhrysheet - 1, column_dhrysheet + 4,     'DhrystoneScore3')
    worksheet_dhrystone.write(row_dhrysheet - 1, column_dhrysheet + 5,     'DhrystoneScore4')
    worksheet_dhrystone.write(row_dhrysheet - 1, column_dhrysheet + 6,     'DeltaTime')

    # filling in dhrystone test logs to worksheet
    for Start,End,DhrystoneScore1,DhrystoneScore2,DhrystoneScore3,DhrystoneScore4,DeltaTime in (log_table):
        worksheet_dhrystone.write(row_dhrysheet, column_dhrysheet,     Start)
        worksheet_dhrystone.write(row_dhrysheet, column_dhrysheet + 1, End)
        worksheet_dhrystone.write(row_dhrysheet, column_dhrysheet + 2, DhrystoneScore1)
        worksheet_dhrystone.write(row_dhrysheet, column_dhrysheet + 3, DhrystoneScore2)
        worksheet_dhrystone.write(row_dhrysheet, column_dhrysheet + 4, DhrystoneScore3)
        worksheet_dhrystone.write(row_dhrysheet, column_dhrysheet + 5, DhrystoneScore4)
        worksheet_dhrystone.write(row_dhrysheet, column_dhrysheet + 6, DeltaTime)
        row_dhrysheet += 1

    calculate_dhrystone_average()

def calculate_dhrystone_average():
    global worksheet_dhrystone

	# default row and column position
    row_dhrysheet    = 2
    column_dhrysheet = 8

    worksheet_dhrystone.write(row_dhrysheet, column_dhrysheet + 1, 'DhrystoneScore1')
    worksheet_dhrystone.write(row_dhrysheet, column_dhrysheet + 2, 'DhrystoneScore2')
    worksheet_dhrystone.write(row_dhrysheet, column_dhrysheet + 3, 'DhrystoneScore3')
    worksheet_dhrystone.write(row_dhrysheet, column_dhrysheet + 4, 'DhrystoneScore4')
    worksheet_dhrystone.write(row_dhrysheet, column_dhrysheet + 5, 'Average_of_Averages')

    worksheet_dhrystone.write(row_dhrysheet + 1, column_dhrysheet, 'Average (without_first_iteration)')
    worksheet_dhrystone.write(row_dhrysheet + 2, column_dhrysheet, 'Total_Average')
    worksheet_dhrystone.write_formula('J4', '=AVERAGE(C3:C6)')
    worksheet_dhrystone.write_formula('K4', '=AVERAGE(D3:D6)')
    worksheet_dhrystone.write_formula('L4', '=AVERAGE(E3:E6)')
    worksheet_dhrystone.write_formula('M4', '=AVERAGE(F3:F6)')

    worksheet_dhrystone.write_formula('J5', '=AVERAGE(C2:C6)')
    worksheet_dhrystone.write_formula('K5', '=AVERAGE(D2:D6)')
    worksheet_dhrystone.write_formula('L5', '=AVERAGE(E2:E6)')
    worksheet_dhrystone.write_formula('M5', '=AVERAGE(F2:F6)')

    worksheet_dhrystone.write_formula('N4', '=AVERAGE(J4:M4)')
    worksheet_dhrystone.write_formula('N5', '=AVERAGE(J5:M5)')

def conv(x):
    return x.replace(',', '.').encode()

if __name__ == "__main__":
        main()
