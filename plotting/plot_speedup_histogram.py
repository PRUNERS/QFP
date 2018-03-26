#!/usr/bin/env python3

# -- LICENSE BEGIN --
#
# Copyright (c) 2015-2018, Lawrence Livermore National Security, LLC.
#
# Produced at the Lawrence Livermore National Laboratory
#
# Written by
#   Michael Bentley (mikebentley15@gmail.com),
#   Geof Sawaya (fredricflinstone@gmail.com),
#   and Ian Briggs (ian.briggs@utah.edu)
# under the direction of
#   Ganesh Gopalakrishnan
#   and Dong H. Ahn.
#
# LLNL-CODE-743137
#
# All rights reserved.
#
# This file is part of FLiT. For details, see
#   https://pruners.github.io/flit
# Please also read
#   https://github.com/PRUNERS/FLiT/blob/master/LICENSE
#
# Redistribution and use in source and binary forms, with or
# without modification, are permitted provided that the following
# conditions are met:
#
# - Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the disclaimer below.
#
# - Redistributions in binary form must reproduce the above
#   copyright notice, this list of conditions and the disclaimer
#   (as noted below) in the documentation and/or other materials
#   provided with the distribution.
#
# - Neither the name of the LLNS/LLNL nor the names of its
#   contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
# CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL
# SECURITY, LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGE.
#
# Additional BSD Notice
#
# 1. This notice is required to be provided under our contract
#    with the U.S. Department of Energy (DOE). This work was
#    produced at Lawrence Livermore National Laboratory under
#    Contract No. DE-AC52-07NA27344 with the DOE.
#
# 2. Neither the United States Government nor Lawrence Livermore
#    National Security, LLC nor any of their employees, makes any
#    warranty, express or implied, or assumes any liability or
#    responsibility for the accuracy, completeness, or usefulness of
#    any information, apparatus, product, or process disclosed, or
#    represents that its use would not infringe privately-owned
#    rights.
#
# 3. Also, reference herein to any specific commercial products,
#    process, or services by trade name, trademark, manufacturer or
#    otherwise does not necessarily constitute or imply its
#    endorsement, recommendation, or favoring by the United States
#    Government or Lawrence Livermore National Security, LLC. The
#    views and opinions of authors expressed herein do not
#    necessarily state or reflect those of the United States
#    Government or Lawrence Livermore National Security, LLC, and
#    shall not be used for advertising or product endorsement
#    purposes.
#
# -- LICENSE END --

'''
Plots the best speedup vs compilation as a bar chart.  The x-axis is the test
name, and the y-axis is the speedup.  There is one bar per compiler (for
fastest safe compilation) and one bar for all unsafe compilations.
'''

import argparse
import csv 
import numpy as np
import os
import sqlite3
import sys

from plot_timing import read_sqlite

# This matplot command makes possible the use of pyplot without X11
import matplotlib
#matplotlib.use('Agg')
import matplotlib.pyplot as plt

def calc_speedups(rows, test_names):
    '''
    Calculates the safe speedup and the unsafe speedup for each test_name,
    compiler combination.
    
    @return (safe_speedups, unsafe_speedups)
    
    - safe_speedups: (dict{compiler -> list[speedup for test_name i]})
      Safe speedups, defined by row['comparison_d'] == 0.0
    - unsafe_speedups: (dist{compiler -> list[speedup for test_name i]})
      Unsafe speedups, defined by not safe.

    If there are no safe runs or no unsafe runs for a given compiler, then the
    speedup returned is zero.  This speedup is against the slowest run across
    compilers.
    '''
    compilers = sorted(set(x['compiler'] for x in rows))

    test_row_map = {x: [row for row in rows if row['name'] == x]
                    for x in test_names}
    safe_speedups = {x: [] for x in compilers}
    unsafe_speedups = {x: [] for x in compilers}
    for test_name in test_names:
        test_rows = test_row_map[test_name]
        compiler_row_map = {x: [row for row in test_rows
                                    if row['compiler'] == x]
                            for x in compilers}
        slowest_time = max([int(x['nanosec']) for x in test_rows])
        fastest_safe = []
        fastest_unsafe = []
        for compiler in compilers:
            compiler_rows = compiler_row_map[compiler]
            safe_times = [int(x['nanosec']) for x in compiler_rows 
                          if float(x['comparison_d']) == 0.0]
            unsafe_times = [int(x['nanosec']) for x in compiler_rows 
                            if float(x['comparison_d']) == 0.0]

            if len(safe_times) > 0:
                safe_speedup = slowest_time / min(safe_times)
            else:
                safe_speedup = 0

            if len(unsafe_times) > 0:
                unsafe_speedup = slowest_time / min(unsafe_times)
            else:
                unsafe_speedup = 0

            safe_speedups[compiler].append(safe_speedup)
            unsafe_speedups[compiler].append(unsafe_speedup)
    return safe_speedups, unsafe_speedups

def plot_histogram(rows, test_names=[], outdir='.'):
    '''
    Plots the timing metrics from the rows and for the given test_names.  The
    resultant plots will be placed in outdir.

    If test_names is empty, then all tests in the rows will be plotted.
    '''
    # Make sure test_names are found in the rows.
    # Also, if test_names is empty, plot all tests
    all_test_names = set(x['name'] for x in rows)
    if len(test_names) == 0:
        test_names = sorted(all_test_names)
    assert all(x in all_test_names for x in test_names), \
            'unfound test names detected'
    
    # Make sure outdir exists
    try:
        os.makedirs(outdir)
    except FileExistsError:
        pass

    safe_speedups, unsafe_speedups = calc_speedups(rows, test_names)
    compilers = safe_speedups.keys()

    width = 1 / (len(compilers) + 2)  # The bar width
    ind = np.arange(len(test_names))  # The x locations for the groups

    #fig = plt.figure(num=1, figsize=(2 + len(test_names), 6))
    #ax = fig.add_axes()
    fig, ax = plt.subplots()
    fig.set_figwidth(2 + len(test_names))
    fig.set_figheight(6)
    bar_colormap = matplotlib.colors.LinearSegmentedColormap(
            'myblues',
            {
                'red': [(0.0, 0x70/256, 0x70/256),
                        (0.5, 0.0, 0.0),
                        (1.0, 0.0, 0.0)],
                'green': [(0.0, 0xdb/256, 0xdb/256),
                          (0.5, 0xad/256, 0xad/256),
                          (1.0, 0x5b/256, 0x5b/256)],
                'blue': [(0.0, 1.0, 1.0),
                         (0.5, 0xda/256, 0xda/256),
                         (1.0, 0xa9/256, 0xa9/256)]
            })
    #bar_colors = plt.cm.Blues(np.linspace(1.0, 0.5, len(compilers)))
    bar_colors = bar_colormap(np.linspace(0.0, 1.0, len(compilers)))
    compiler_rects = [ax.bar(ind + width*i, safe_speedups[comp], width, color=bar_colors[i])
                      for i, comp in enumerate(compilers)]
    unsafe_rect = ax.bar(ind + width*len(compilers),
                         [max(unsafe_speedups[comp][i] for comp in compilers)
                          for i in range(len(test_names))],
                         width,
                         color='r')

    ax.set_ylabel('Speedup from Slowest')
    ax.yaxis.grid(which='major')  # Have horizontal grid lines
    ax.set_axisbelow(True)        # Grid lines behind the bars
    ax.set_xticks(ind - width)
    ax.set_xticklabels(test_names)

    legend = ax.legend(
            compiler_rects + [unsafe_rect],
            [x + ' fastest safe' for x in compilers] + ['fastest unsafe'])
    legend.get_frame().set_alpha(1.0)

    plt.setp(ax.xaxis.get_majorticklabels(), rotation=-45, ha='left')

    dx = 0.5 * (1 - width)
    offset = matplotlib.transforms.ScaledTranslation(dx, 0, fig.dpi_scale_trans)
    
    for label in ax.xaxis.get_majorticklabels():
        label.set_transform(label.get_transform() + offset)

    plt.tight_layout()
    plt.savefig(os.path.join(outdir, 'tmp.svg'), format='svg')
    #plt.show()

    # -------
    
#    for name in test_names:
#        test_rows = [row for row in rows if row['name'] == name]
#        hosts = set(row['host'] for row in test_rows)
#        for host in hosts:
#            host_rows = [row for row in test_rows if row['host'] == host]
#            precisions = set(row['precision'] for row in host_rows)
#            for p in precisions:
#                p_rows = [row for row in host_rows if row['precision'] == p]
#                data = {}
#                data['name'] = name
#                data['rows'] = p_rows
#                data['times'] = np.asarray([int(row['nanosec'])
#                                            for row in data['rows']])
#                data['fastest'] = min(data['times'])
#                data['slowest'] = max(data['times'])
#                # TODO: instead of calculating the speedup using the slowest
#                # TODO: time, use the ground-truth time.
#                data['speedup'] = data['slowest'] / data['times']
#                data['xlab'] = [to_x_label(row) for row in data['rows']]
#                data['iseql'] = [float(row['comparison_d']) == 0.0
#                                 for row in data['rows']]
#                key = (name, host, p)
#                test_data[key] = data
#
#    for key, data in test_data.items():
#        name, host, p = key
#        print(name, max(data['speedup']))
#        print('  slowest', data['slowest'])
#        print('  fastest', data['fastest'])
#        speedup = data['speedup']
#        xlab = data['xlab']
#        iseql = data['iseql']
#        joint_sort = sorted(zip(xlab, speedup, iseql), key=lambda x: x[1])
#        xlab = [x[0] for x in joint_sort]
#        speedup = [x[1] for x in joint_sort]
#        iseql = [x[2] for x in joint_sort]
#        eql_idxs = [i for i in range(len(iseql)) if iseql[i] is True]
#        not_eql_idxs = [i for i in range(len(iseql)) if iseql[i] is False]
#        eql_speeds = [speedup[i] for i in eql_idxs]
#        not_eql_speeds = [speedup[i] for i in not_eql_idxs]
#
#        plt.figure(num=1, figsize=(12.5,5), dpi=80)
#        plt.plot(speedup)
#        plt.plot(eql_idxs, eql_speeds, 'b.',
#                 label='same answer as ground truth')
#        plt.plot(not_eql_idxs, not_eql_speeds, 'rx',
#                 label='different answer than ground truth')
#        plt.xticks(np.arange(len(xlab)), xlab, rotation='vertical')
#        plt.legend()
#        plt.ylim(ymin=0)
#        plt.ylabel('Speedup from slowest')
#        plt.tight_layout()
#        newname = '{0}-{1}-{2}.svg'.format(name, host, p)
#        plt.savefig(os.path.join(outdir, newname), format='svg')
#        plt.cla()

def main(arguments):
    'Main entry point, calls plot_timing()'
    parser = argparse.ArgumentParser()
    parser.add_argument('-o', '--outdir', default='.',
            help='Specify output directory for generated plots')
    parser.add_argument('-r', '--run', default=None, type=int,
            help='Which run to use from the sqlite database')
    parser.add_argument('-p', '--precision', default='all',
            choices=['all', 'f', 'd', 'e'],
            help='Which precision to draw.  By default does all precisions')
    parser.add_argument('sqlite')
    parser.add_argument('test', nargs='*')
    args = parser.parse_args(arguments)
    rows = read_sqlite(args.sqlite, args.run)
    if args.precision != 'all':
        rows = [x for x in rows if x['precision'] == args.precision]
    plot_histogram(rows, args.test, args.outdir)

if __name__ == '__main__':
    main(sys.argv[1:])