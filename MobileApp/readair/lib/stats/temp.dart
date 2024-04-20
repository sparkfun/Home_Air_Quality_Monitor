import 'package:flutter/material.dart';
import 'package:fl_chart/fl_chart.dart';
import 'dart:math';

import 'package:readair/data/packet.dart';

class TempPage extends StatefulWidget {
  @override
  State<TempPage> createState() => _TempPageState();
}

class _TempPageState extends State<TempPage> {
  bool isCelsius = true; // Default unit is Celsius
  double? currentTemp;
  double? averageTemp;
  double? maxTemp;
  double? minTemp;
  List<FlSpot> tempSpots = [];

  @override
  void initState() {
    super.initState();
    fetchTempData();
  }


  Future<void> fetchTempData() async {
    List<DataPacket> lastTwentyFourHourPackets =
        await DatabaseService.instance.getPacketsForLastHours(24);

    if (lastTwentyFourHourPackets.isNotEmpty) {
      currentTemp = lastTwentyFourHourPackets.first.temp;

      double totalTemp = lastTwentyFourHourPackets.map((packet) => packet.temp).reduce((a, b) => a + b);
      averageTemp = totalTemp / lastTwentyFourHourPackets.length;

      maxTemp = lastTwentyFourHourPackets.map((packet) => packet.temp).reduce(max);
      minTemp = lastTwentyFourHourPackets.map((packet) => packet.temp).reduce(min);

      tempSpots = lastTwentyFourHourPackets.asMap().entries.map((entry) => FlSpot(entry.key.toDouble(), entry.value.temp)).toList();
    }

    setState(() {});
  }
  // Convert Fahrenheit to Celsius
  double fahrenheitToCelsius(int fahrenheit) {
    return (fahrenheit - 32) * 5 / 9;
  }

  Color? TEMPColor(double value) {
    if (value <= 0) {
      return Colors.blue[900];
    } else if (value > 0 && value <= 15) {
      return Colors.blue;
    } else if (value > 15 && value <= 25) {
      return Colors.green;
    } else if (value > 25 && value <= 35) {
      return Colors.orange;
    } else {
      return Colors.red;
    }
  }

  String TEMPmessage(double value) {
    if (value <= 0) {
      return "Very Cold";
    } else if (value > 0 && value <= 15) {
      return "Cold";
    } else if (value > 15 && value <= 25) {
      return "Comfortable";
    } else if (value > 25 && value <= 35) {
      return "Warm";
    } else {
      return "Hot";
    }
  }

  @override
  Widget build(BuildContext context) {

    return Scaffold(
      appBar: AppBar(
        leading: IconButton(
          icon: Icon(Icons.arrow_back),
          onPressed: () => Navigator.of(context).pop(),
        ),
        title: Text("Temperature"),
        actions: [
          // Switch to toggle between Celsius and Fahrenheit
          Padding(
            padding: const EdgeInsets.all(8.0),
            child: Switch(
              value: isCelsius,
              onChanged: (value) {
                setState(() {
                  isCelsius = value;
                });
              },
            ),
          ),
          Padding(
            padding: const EdgeInsets.all(8.0),
            child: Text(isCelsius ? "°C" : "°F"),
          ),
        ],
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
                      color: TEMPColor(currentTemp?.toDouble() ?? 0),
                      child: Column(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          Text('${currentTemp?.toStringAsFixed(1) ?? '-'}°',
                              style: TextStyle(
                                  fontSize: 38, color: Colors.white70)),
                        ],
                      )),
                ),
                Container(
                  width: MediaQuery.of(context).size.width / 2,
                  child: Card(
                      color: TEMPColor(averageTemp?.toDouble() ?? 0),
                      child: Column(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          Text('${averageTemp?.toStringAsFixed(1) ?? '-'}°',
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
                    child: Text('Temperature is ${TEMPmessage(currentTemp?.toDouble() ?? 0)}',
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
                          reservedSize: 40,
                          interval: 10,
                          getTitlesWidget: (value, meta) {
                            return Text('${value.toInt()}');
                          },
                        ),
                      ),
                      bottomTitles: AxisTitles(
                        sideTitles: SideTitles(
                          showTitles: true,
                          reservedSize: 20,
                          interval: 1,
                          getTitlesWidget: (value, meta) {
                            if (value % 5 == 0) return Text('${value.toInt()}h');
                            return Text('');
                          },
                        ),
                      ),
                    ),
                    borderData: FlBorderData(show: true),
                    lineBarsData: [
                      LineChartBarData(
                        spots: tempSpots,
                        isCurved: true,
                        dotData: FlDotData(show: false),
                        color: Colors.blue,
                        barWidth: 3,
                      ),
                    ],
                  ),
                ),
              ),
            ),
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
                      color: TEMPColor(maxTemp?.toDouble() ?? 0),
                      child: Column(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          Text('${maxTemp?.toStringAsFixed(1) ?? '-'}°',
                              style: TextStyle(
                                  fontSize: 38, color: Colors.white70)),
                        ],
                      )),
                ),
                Container(
                  width: MediaQuery.of(context).size.width / 2,
                  child: Card(
                      color: TEMPColor(minTemp?.toDouble() ?? 0),
                      child: Column(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          Text('${minTemp?.toStringAsFixed(1) ?? '-'}°',
                              style: TextStyle(
                                  fontSize: 38, color: Colors.white70)),
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
                    child: Text('Temperature Index (F\u00B0)',
                        style: TextStyle(fontSize: 30))),
              ),
            ),

            const Card(
              color: Color.fromARGB(255, 48, 49, 133),
              child: Row(
                children: [
                  Align(
                    alignment: Alignment.centerLeft,
                    child: Padding(
                      padding: EdgeInsets.all(8.0),
                      child: Text('Very Cold',
                          style: TextStyle(fontSize: 28, color: Colors.white70),
                          textAlign: TextAlign.left),
                    ),
                  ),
                  Spacer(),
                  Padding(
                    padding: EdgeInsets.all(8.0),
                    child: Text('31 & Below',
                        style: TextStyle(fontSize: 30, color: Colors.white70),
                        textAlign: TextAlign.right),
                  ),
                ],
              ),
            ),

            const Card(
              color: Color.fromARGB(255, 13, 146, 229),
              child: Row(
                children: [
                  Align(
                    alignment: Alignment.centerLeft,
                    child: Padding(
                      padding: EdgeInsets.all(8.0),
                      child: Text('Cold',
                          style: TextStyle(fontSize: 30, color: Colors.white70),
                          textAlign: TextAlign.left),
                    ),
                  ),
                  Spacer(),
                  Padding(
                    padding: EdgeInsets.all(8.0),
                    child: Text('32 - 58',
                        style: TextStyle(fontSize: 30, color: Colors.white70),
                        textAlign: TextAlign.right),
                  ),
                ],
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
                      child: Text('Comfortable',
                          style: TextStyle(fontSize: 30, color: Colors.white70),
                          textAlign: TextAlign.left),
                    ),
                  ),
                  Spacer(),
                  Padding(
                    padding: EdgeInsets.all(8.0),
                    child: Text('59-71',
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
                      child: Text('Warm',
                          style: TextStyle(fontSize: 30, color: Colors.white70),
                          textAlign: TextAlign.left),
                    ),
                  ),
                  Spacer(),
                  Padding(
                    padding: EdgeInsets.all(8.0),
                    child: Text('72-94',
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
                      child: Text('Hot',
                          style: TextStyle(fontSize: 30, color: Colors.white70),
                          textAlign: TextAlign.left),
                    ),
                  ),
                  Spacer(),
                  Padding(
                    padding: EdgeInsets.all(8.0),
                    child: Text('95 & Above',
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
