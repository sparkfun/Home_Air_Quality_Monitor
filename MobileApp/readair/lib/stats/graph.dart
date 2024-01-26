import 'package:flutter/material.dart';
import 'package:fl_chart/fl_chart.dart';
import 'package:intl/intl.dart';
import 'dart:math';

class DataPoint {
  final double measurement;
  final int epoch;

  DataPoint(this.measurement, this.epoch);
}

class GraphWidget extends StatelessWidget {
  final String title;
  final List<DataPoint> dataPoints;

  const GraphWidget({Key? key, required this.title, required this.dataPoints})
      : super(key: key);

  @override
  Widget build(BuildContext context) {
    // Ensuring only 100 datasets are shown
    final points =
        dataPoints.length > 100 ? dataPoints.sublist(0, 100) : dataPoints;
    final double minY = points.isNotEmpty
        ? max(points.map((e) => e.measurement).reduce(min) - 100.0, 0.0)
        : 0.0;

    final maxY = points.isNotEmpty
        ? points.map((e) => e.measurement).reduce(max) + 100.0
        : 100.0;

    // Calculate y-axis interval
    final yInterval = ((maxY - minY) / 4);

    return Column(
      children: [
        Padding(
          padding: const EdgeInsets.symmetric(vertical: 8.0),
          child: Text(title,
              style: TextStyle(fontSize: 24, fontWeight: FontWeight.bold)),
        ),
        Container(
          height: 300,
          child: LineChart(
            LineChartData(
              gridData: FlGridData(show: false),
              titlesData: FlTitlesData(
                leftTitles: AxisTitles(
                  sideTitles: SideTitles(
                    showTitles: true,
                    reservedSize: 40,
                    getTitlesWidget: (value, meta) {
                      return Text('${value.toInt()}');
                    },
                    interval: yInterval, // Y-Axis interval
                  ),
                ),
                bottomTitles: AxisTitles(
                  sideTitles: SideTitles(
                    showTitles: true,
                    reservedSize: 20,
                    getTitlesWidget: (value, meta) {
                      // Display only 5 labels on X-Axis
                      if (value % (points.length / 4) == 0 ||
                          value == points.length - 1) {
                        final DateTime date =
                            DateTime.fromMillisecondsSinceEpoch(
                                points[value.toInt()].epoch * 1000);
                        return Padding(
                          padding: const EdgeInsets.only(top: 10.0),
                          child: Text(DateFormat('MM/dd').format(date),
                              style: TextStyle(fontSize: 10)),
                        );
                      } else {
                        return Container();
                      }
                    },
                    interval: 1, // To show every point's label
                  ),
                ),
                topTitles: AxisTitles(
                  // Hide top titles
                  sideTitles: SideTitles(showTitles: false),
                ),
                rightTitles: AxisTitles(
                  // Hide right titles
                  sideTitles: SideTitles(showTitles: false),
                ),
              ),
              borderData: FlBorderData(
                show: true,
                border: Border(
                  bottom: BorderSide(
                      color: Colors.black, width: 2), // Show bottom side
                  left: BorderSide(
                      color: Colors.black, width: 2), // Show left side
                  top: BorderSide.none, // Hide top side
                  right: BorderSide.none, // Hide right side
                ),
              ),
              lineBarsData: [
                LineChartBarData(
                  spots: _createSpots(points),
                  isCurved: false, // Set this to false to remove curve effect
                  // Or set this to true and adjust the below property
                  // curveSmoothness: 0.1, // Experiment with this value to reduce curve effect
                  dotData: FlDotData(
                      show: true,
                      getDotPainter: (spot, percent, barData, index) {
                        return FlDotCirclePainter(
                          radius: 4,
                          color: Colors.blue,
                          strokeWidth: 1.5,
                          strokeColor: Colors.white,
                        );
                      }),
                  belowBarData: BarAreaData(show: false),
                  color: Colors.blue,
                  barWidth: 3,
                ),
              ],
              minY: minY,
              maxY: maxY,
              lineTouchData: LineTouchData(
                touchTooltipData: LineTouchTooltipData(
                  tooltipBgColor: Colors.blueAccent,
                  getTooltipItems: (touchedSpots) {
                    return touchedSpots.map((touchedSpot) {
                      final DateTime date = DateTime.fromMillisecondsSinceEpoch(
                          points[touchedSpot.spotIndex].epoch * 1000);
                      return LineTooltipItem(
                        '${touchedSpot.y} at ${DateFormat('MM/dd/yyyy').format(date)}',
                        const TextStyle(color: Colors.white),
                      );
                    }).toList();
                  },
                ),
              ),
            ),
          ),
        ),
      ],
    );
  }

  List<FlSpot> _createSpots(List<DataPoint> points) {
    return List.generate(points.length,
        (index) => FlSpot(index.toDouble(), points[index].measurement));
  }
}
