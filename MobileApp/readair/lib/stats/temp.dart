// ignore_for_file: non_constant_identifier_names

import 'package:flutter/material.dart';
import 'package:fl_chart/fl_chart.dart';
import 'dart:math';

class TempPage extends StatefulWidget {
  // const AQIPage({super.key, required this.title});

  // final String title;

  @override
  State<TempPage> createState() => _TempPageState();
}

class _TempPageState extends State<TempPage> {
  final Random _random = Random();
  List<FlSpot> generateRandomData() {
    return List.generate(
        10, (index) => FlSpot(index.toDouble(), _random.nextInt(90) + 10.0));
  }

  Color? TEMPColor(int value) {
    if (value <= 31) {
      return Color.fromARGB(255, 48, 49, 133);
    } else if (value > 32 && value <= 58) {
      return Color.fromARGB(255, 13, 146, 229);
    } else if (value > 59 && value <= 71) {
      return Color.fromARGB(255, 48, 133, 56);
    } else if (value > 72 && value <= 95) {
      return Color.fromARGB(255, 229, 114, 13);
    } else {
      return Color.fromARGB(255, 217, 19, 4);
    }
  }

  int TEMPcurrentValue = 55;
  int TEMPhour = 43;
  int TEMPmax = 67;
  int TEMPmin = 22;

  String TEMPmessage(int value) {
    if (value <= 31) {
      return "Extremely Cold";
    } else if (value > 32 && value <= 58) {
      return "Cold";
    } else if (value > 59 && value <= 71) {
      return "Moderate";
    } else if (value > 72 && value <= 95) {
      return "Moderately Warm";
    } else {
      return "Hot";
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
        title: Text("Temperature"),
      ),
      body: SingleChildScrollView(
        child: Column(
          children: [
            const SizedBox(height: 10), //Spacing between the "boxes"
            Padding(
              padding: EdgeInsets.all(8.0),
              child: Card(
                //IF STATEMENT! Change color with Quality of Air

                color: TEMPColor(TEMPcurrentValue),
                child: ListTile(
                  title: Center(
                      child: Text('$TEMPcurrentValue°F',
                          style: TextStyle(fontSize: 60))),
                  textColor: Colors.white70,

                  //trailing: Icon(Icons.wb_sunny, size: 40),
                ),
              ),
            ),

            Padding(
              padding: EdgeInsets.all(2.0),
              child: ListTile(
                title: Center(
                    child: Text(
                        'The Temperature is ${TEMPmessage(TEMPcurrentValue)}',
                        style: TextStyle(fontSize: 25))),

                //trailing: Icon(Icons.wb_sunny, size: 40),
              ),
            ),

            //SizedBox(height: 10), //spacing beween the "boxes"
            const Divider(
              thickness: 3,
              indent: 20,
              endIndent: 20,
            ),
            // add more stuff here!!
            //const SizedBox(height: 2), //Spacing between the "boxes"
            const Padding(
              padding: EdgeInsets.all(6.0),
              child: ListTile(
                title: Center(
                    child:
                        Text('24 Hour Span', style: TextStyle(fontSize: 30))),

                //trailing: Icon(Icons.wb_sunny, size: 40),
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
                        spots: data,
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

            const Padding(
              padding: EdgeInsets.all(2.0),
              child: ListTile(
                title: Center(
                    child: Text('Average Over Past 24hrs',
                        style: TextStyle(fontSize: 25))),
              ),
            ),

            Padding(
              padding: EdgeInsets.all(8.0),
              child: Card(
                //IF STATEMENT! Change color with Quality of Air
                color: TEMPColor(TEMPhour),
                child: ListTile(
                  title: Center(
                      child: Text('$TEMPhour\u00B0F',
                          style: TextStyle(fontSize: 50))),
                  textColor: Colors.white70,
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
                    style: TextStyle(fontSize: 25, color: Colors.black),
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
                      color: TEMPColor(TEMPmax),
                      child: Column(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          Text('$TEMPmax\u00B0F',
                              style: TextStyle(
                                  fontSize: 50, color: Colors.white70)),
                        ],
                      )),
                ),
                Container(
                  width: MediaQuery.of(context).size.width / 2,
                  child: Card(
                      color: TEMPColor(TEMPmin),
                      child: Column(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          Text('$TEMPmin\u00B0F',
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
                    child: Text('Temperature Index (F\u00B0)',
                        style: TextStyle(fontSize: 35))),
              ),
            ),

            //const SizedBox(height: 3), //Spacing between the "boxes"
            const Card(
              //IF STATEMENT! Change color with Quality of Air
              color: Color.fromARGB(255, 48, 49, 133),
              child: // ListTile(
                  //title:
                  Row(
                children: [
                  Align(
                    alignment: Alignment.centerLeft,
                    child: Padding(
                      padding: EdgeInsets.all(8.0),
                      child: Text('Extremely Cold',
                          style: TextStyle(fontSize: 30, color: Colors.white70),
                          textAlign: TextAlign.left),
                    ),
                  ),
                  Spacer(),
                  Padding(
                    padding: EdgeInsets.all(8.0),
                    child: Text('less than 31',
                        style: TextStyle(fontSize: 30, color: Colors.white70),
                        textAlign: TextAlign.right),
                  ),
                ],
              ),
            ),

            //const SizedBox(height: 3), //Spacing between the "boxes"
            const Card(
              //IF STATEMENT! Change color with Quality of Air
              color: Color.fromARGB(255, 13, 146, 229),
              child: // ListTile(
                  //title:
                  Row(
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

            //const SizedBox(height: 3), //Spacing between the "boxes"
            const Card(
              //IF STATEMENT! Change color with Quality of Air
              color: Color.fromARGB(255, 48, 133, 56),
              child: // ListTile(
                  //title:
                  Row(
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
                    child: Text('59-71',
                        style: TextStyle(fontSize: 30, color: Colors.white70),
                        textAlign: TextAlign.right),
                  ),
                ],
              ),
            ),

            //const SizedBox(height: 3), //Spacing between the "boxes"
            const Card(
              //IF STATEMENT! Change color with Quality of Air
              color: Color.fromARGB(255, 229, 114, 13),
              child: // ListTile(
                  //title:
                  Row(
                children: [
                  Align(
                    alignment: Alignment.centerLeft,
                    child: Padding(
                      padding: EdgeInsets.all(8.0),
                      child: Text('Moderately Warm',
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

            //const SizedBox(height: 3), //Spacing between the "boxes"
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
                    child: Text('greater than 95',
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
