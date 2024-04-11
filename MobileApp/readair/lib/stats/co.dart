// ignore_for_file: non_constant_identifier_names

import 'package:flutter/material.dart';
import 'package:fl_chart/fl_chart.dart';
import 'dart:math';

import 'package:readair/data/packet.dart';

class COPage extends StatefulWidget {
  @override
  State<COPage> createState() => _COPageState();
}

class _COPageState extends State<COPage> {
  final Random _random = Random();
  List<FlSpot> generateRandomData() {
    return List.generate(
        10, (index) => FlSpot(index.toDouble(), _random.nextInt(90) + 10.0));
  }

  double? current;
  double? average;
  double? maxVal;
  double? minVal;
  List<FlSpot> valSpots = [];

      @override
  void initState() {
    super.initState();
    fetchValData();
  }

    Future<void> fetchValData() async {
    List<DataPacket> lastTwentyFourHourPackets =
        await DatabaseService.instance.getPacketsForLastHours(24);

    if (lastTwentyFourHourPackets.isNotEmpty) {
      current = lastTwentyFourHourPackets.first.co;

      double totalVal = lastTwentyFourHourPackets.map((packet) => packet.ng).reduce((a, b) => a + b);
      average = totalVal / lastTwentyFourHourPackets.length;

      maxVal = lastTwentyFourHourPackets.map((packet) => packet.co).reduce(max);
      minVal = lastTwentyFourHourPackets.map((packet) => packet.co).reduce(min);

      valSpots = lastTwentyFourHourPackets.asMap().entries.map((entry) => FlSpot(entry.key.toDouble(), entry.value.co)).toList();
    }

    setState(() {});
  }

  Color? CoordinatedColor(int value) {
    //Colors cordinated with the danger levels
    if (value <= 36) {
      return Color.fromARGB(255, 48, 133, 56);
    } else if (value > 36 && value <= 100) {
      return Color.fromARGB(255, 229, 193, 13);
    } else if (value > 100 && value <= 200) {
      return Color.fromARGB(255, 229, 114, 13);
    } else if (value > 200 && value <= 400) {
      return Color.fromARGB(255, 217, 19, 4);
    } else {
      return Color.fromARGB(255, 121, 0, 0);
    }
  }

  String message(int value) {
    //Displays message below current value
    if (value <= 36) {
      return "Good";
    } else if (value > 36 && value <= 100) {
      return "Moderate";
    } else if (value > 100 && value <= 200) {
      return "Bad";
    } else if (value > 200 && value <= 400) {
      return "Unhealthy";
    } else {
      return "Dangerous";
    }
  }

  @override
  Widget build(BuildContext context) {
    List<FlSpot> data = generateRandomData();
    return Scaffold(
      appBar: AppBar(
        leading: IconButton(
          icon: Icon(Icons.arrow_back),
          onPressed: () => Navigator.of(context).pop(),
        ),
        title: Text("Carbon Monoxide (ppm)"),
      ),
      body: SingleChildScrollView(
        child: Column(
          children: [
            const SizedBox(height: 10), //Spacing between the "boxes"
             const Row(
              mainAxisAlignment: MainAxisAlignment.spaceAround,
              children: [
                Padding(
                  padding: EdgeInsets.all(8.0),
                  child: Text(
                    'Current value',
                    style: TextStyle(fontSize: 20),
                  ),
                ),
                Padding(
                  padding: EdgeInsets.all(8.0),
                  child: Text('Average value', style: TextStyle(fontSize: 20)),
                ),
              ],
            ),

            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                Container(
                  width: MediaQuery.of(context).size.width / 2,
                  child: Card(
                      color: CoordinatedColor(current?.toInt() ?? 0),
                      child: Column(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          Text('${current?.toStringAsFixed(1) ?? '-'}',
                              style: TextStyle(
                                  fontSize: 38, color: Colors.white70)),
                        ],
                      )),
                ),
                Container(
                  width: MediaQuery.of(context).size.width / 2,
                  child: Card(
                      color: CoordinatedColor(average?.toInt() ?? 0),
                      child: Column(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          Text('${average?.toStringAsFixed(1) ?? '-'}',
                              style: TextStyle(
                                  fontSize: 38, color: Colors.white70)),
                        ],
                      )),
                ),
              ],
            ),
            
            Padding(
              padding: EdgeInsets.all(2.0),
              child: ListTile(
                title: Center(
                    child: Text('Carbon Monoxide is ${message(current?.toInt() ?? 0)}',
                        style: TextStyle(fontSize: 25, fontWeight: FontWeight.bold))),
              ),
            ),

            const Divider(
              thickness: 3,
              indent: 20,
              endIndent: 20,
            ),

            const Padding(
              padding: EdgeInsets.all(6.0),
              child: ListTile(
                title: Center(
                    child:
                        Text('24 Hour Span', style: TextStyle(fontSize: 30))),
              ),
            ),

            Padding(
              padding: const EdgeInsets.all(8.0),
              child: Container(
                height: 300,
                child: LineChart(
                  LineChartData(
                    gridData: FlGridData(show: false),
                    titlesData: FlTitlesData(
                      leftTitles: AxisTitles(
                        sideTitles: SideTitles(
                          showTitles: true,
                          getTitlesWidget: (value, meta) {
                            if (value % 10 == 0)
                              return Text('${value.toInt()}');
                            return Text('');
                          },
                          reservedSize: 40,
                        ),
                      ),
                      bottomTitles: AxisTitles(
                        sideTitles: SideTitles(
                          showTitles: true,
                          getTitlesWidget: (value, meta) {
                            return Text('${value.toInt()}');
                          },
                          reservedSize: 20,
                        ),
                      ),
                    ),
                    borderData: FlBorderData(show: true),
                    lineBarsData: [
                      LineChartBarData(
                        spots: valSpots,
                        isCurved: true,
                        dotData: FlDotData(show: false),
                        belowBarData: BarAreaData(show: false),
                        color: Colors.blue,
                        barWidth: 3,
                      ),
                    ],
                  ),
                ),
              ),
            ),

            const Divider(
              thickness: 3,
              indent: 20,
              endIndent: 20,
            ),

            // const Padding(
            //   padding: EdgeInsets.all(2.0),
            //   child: ListTile(
            //     title: Center(
            //         child: Text('Average Over Past 24hrs',
            //             style: TextStyle(fontSize: 25))),
            //   ),
            // ),

            // Padding(
            //   padding: EdgeInsets.all(8.0),
            //   child: Card(
            //     color: CoordinatedColor(averageOverTwoFourHour),
            //     child: ListTile(
            //       title: Center(
            //           child: Text('$averageOverTwoFourHour (ppm)',
            //               style: TextStyle(fontSize: 50))),
            //       textColor: Colors.white70,
            //     ),
            //   ),
            // ),

            const Row(
              mainAxisAlignment: MainAxisAlignment.spaceAround,
              children: [
                Padding(
                  padding: EdgeInsets.all(8.0),
                  child: Text(
                    'Maximum',
                    style: TextStyle(fontSize: 25),
                  ),
                ),
                Padding(
                  padding: EdgeInsets.all(8.0),
                  child: Text('Minimum', style: TextStyle(fontSize: 25)),
                ),
              ],
            ),

            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                Container(
                  width: MediaQuery.of(context).size.width / 2,
                  child: Card(
                      color: CoordinatedColor(maxVal?.toInt() ?? 0),
                      child: Column(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          Text('${maxVal?.toStringAsFixed(1) ?? '-'}',
                              style: TextStyle(
                                  fontSize: 50, color: Colors.white70)),
                        ],
                      )),
                ),
                Container(
                  width: MediaQuery.of(context).size.width / 2,
                  child: Card(
                      color: CoordinatedColor(minVal?.toInt() ?? 0),
                      child: Column(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          Text('${minVal?.toStringAsFixed(1) ?? '-'}',
                              style: TextStyle(
                                  fontSize: 50, color: Colors.white70)),
                        ],
                      )),
                ),
              ],
            ),

            const Divider(
              thickness: 3,
              indent: 20,
              endIndent: 20,
            ),

            const Padding(
              padding: EdgeInsets.all(6.0),
              child: ListTile(
                title: Center(
                    child: Text('Carbon Monoxide (ppm)',
                        style: TextStyle(fontSize: 31))),
              ),
            ),

            const Card(
              color: Color.fromARGB(255, 48, 133, 56),
              child: Row(
                children: [
                  Align(
                    alignment: Alignment.centerLeft,
                    child: Padding(
                      padding: EdgeInsets.all(8.0),
                      child: Text('Good',
                          style: TextStyle(fontSize: 30, color: Colors.white70),
                          textAlign: TextAlign.left),
                    ),
                  ),
                  Spacer(),
                  Padding(
                    padding: EdgeInsets.all(8.0),
                    child: Text('0-36',
                        style: TextStyle(fontSize: 30, color: Colors.white70),
                        textAlign: TextAlign.right),
                  ),
                ],
              ),
            ),

            const Card(
              color: Color.fromARGB(255, 229, 193, 13),
              child: Row(
                children: [
                  Align(
                    alignment: Alignment.centerLeft,
                    child: Padding(
                      padding: EdgeInsets.all(8.0),
                      child: Text('Moderate',
                          style: TextStyle(fontSize: 30, color: Colors.white70),
                          textAlign: TextAlign.left),
                    ),
                  ),
                  Spacer(),
                  Padding(
                    padding: EdgeInsets.all(8.0),
                    child: Text('37-100',
                        style: TextStyle(fontSize: 30, color: Colors.white70),
                        textAlign: TextAlign.right),
                  ),
                ],
              ),
            ),

            const Card(
              color: Color.fromARGB(255, 229, 114, 13),
              child: Row(
                children: [
                  Align(
                    alignment: Alignment.centerLeft,
                    child: Padding(
                      padding: EdgeInsets.all(8.0),
                      child: Text('Bad',
                          style: TextStyle(fontSize: 30, color: Colors.white70),
                          textAlign: TextAlign.left),
                    ),
                  ),
                  Spacer(),
                  Padding(
                    padding: EdgeInsets.all(8.0),
                    child: Text('101-200',
                        style: TextStyle(fontSize: 30, color: Colors.white70),
                        textAlign: TextAlign.right),
                  ),
                ],
              ),
            ),

            const Card(
              color: Color.fromARGB(255, 217, 19, 4),
              child: Row(
                children: [
                  Align(
                    alignment: Alignment.centerLeft,
                    child: Padding(
                      padding: EdgeInsets.all(8.0),
                      child: Text('Unhealthy',
                          style: TextStyle(fontSize: 30, color: Colors.white70),
                          textAlign: TextAlign.left),
                    ),
                  ),
                  Spacer(),
                  Padding(
                    padding: EdgeInsets.all(8.0),
                    child: Text('201-400',
                        style: TextStyle(fontSize: 30, color: Colors.white70),
                        textAlign: TextAlign.right),
                  ),
                ],
              ),
            ),

            const Card(
              color: Color.fromARGB(255, 121, 0, 0),
              child: Row(
                children: [
                  Align(
                    alignment: Alignment.centerLeft,
                    child: Padding(
                      padding: EdgeInsets.all(8.0),
                      child: Text('Dangerous',
                          style: TextStyle(fontSize: 30, color: Colors.white70),
                          textAlign: TextAlign.left),
                    ),
                  ),
                  Spacer(),
                  Padding(
                    padding: EdgeInsets.all(8.0),
                    child: Text('401-12,800',
                        style: TextStyle(fontSize: 30, color: Colors.white70),
                        textAlign: TextAlign.right),
                  ),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}