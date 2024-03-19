import 'package:flutter/material.dart';
import 'package:fl_chart/fl_chart.dart';
import 'dart:math';

class TempPage extends StatefulWidget {
  @override
  State<TempPage> createState() => _TempPageState();
}

class _TempPageState extends State<TempPage> {
  final Random _random = Random();
  bool isCelsius = true; // Default unit is Celsius

  List<FlSpot> generateRandomData() {
    return List.generate(
        10, (index) => FlSpot(index.toDouble(), _random.nextInt(90) + 10.0));
  }

  // Fixed values for testing only, in Fahrenheit
  int currentValue = 32;
  int averageOverTwoFourHour = 43;
  int max = 67;
  int min = 22;

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
    List<FlSpot> data = generateRandomData();
    double displayCurrentValue = isCelsius ? fahrenheitToCelsius(currentValue) : currentValue.toDouble();
    double displayAverage = isCelsius ? fahrenheitToCelsius(averageOverTwoFourHour) : averageOverTwoFourHour.toDouble();
    double displayMax = isCelsius ? fahrenheitToCelsius(max) : max.toDouble();
    double displayMin = isCelsius ? fahrenheitToCelsius(min) : min.toDouble();

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
            const SizedBox(height: 10),
            Padding(
              padding: EdgeInsets.all(8.0),
              child: Card(
                color: TEMPColor(displayCurrentValue),
                child: ListTile(
                  title: Center(
                      child: Text('${displayCurrentValue.toStringAsFixed(1)}${isCelsius ? '°C' : '°F'}',
                          style: TextStyle(fontSize: 60, color: Colors.white))),
                ),
              ),
            ),
            Padding(
              padding: EdgeInsets.all(2.0),
              child: ListTile(
                title: Center(
                  child: Text(
                    'The Temperature is ${TEMPmessage(displayCurrentValue)}',
                    style: TextStyle(fontSize: 25),
                  ),
                ),
              ),
            ),
            const Divider(
              thickness: 3,
              indent: 20,
              endIndent: 20,
            ),
            Padding(
              padding: const EdgeInsets.all(8.0),
              child: Text(
                '24 Hour Temperature Graph',
                style: TextStyle(fontSize: 24, fontWeight: FontWeight.bold),
                textAlign: TextAlign.center,
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
                        spots: data,
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
            Padding(
              padding: EdgeInsets.all(8.0),
              child: Card(
                color: TEMPColor(displayAverage),
                child: ListTile(
                  title: Center(
                    child: Text(
                      'Avg: ${displayAverage.toStringAsFixed(1)}${isCelsius ? '°C' : '°F'}',
                      style: TextStyle(fontSize: 24, color: Colors.white),
                    ),
                  ),
                ),
              ),
            ),
            Row(
              mainAxisAlignment: MainAxisAlignment.spaceEvenly,
              children: [
                Card(
                  color: TEMPColor(displayMax),
                  child: Padding(
                    padding: EdgeInsets.all(8.0),
                    child: Text(
                      'Max: ${displayMax.toStringAsFixed(1)}${isCelsius ? '°C' : '°F'}',
                      style: TextStyle(fontSize: 24, color: Colors.white),
                    ),
                  ),
                ),
                Card(
                  color: TEMPColor(displayMin),
                  child: Padding(
                    padding: EdgeInsets.all(8.0),
                    child: Text(
                      'Min: ${displayMin.toStringAsFixed(1)}${isCelsius ? '°C' : '°F'}',
                      style: TextStyle(fontSize: 24, color: Colors.white),
                    ),
                  ),
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
                      child: Text('Extremely Cold',
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

            const Card(
              color: Color.fromARGB(255, 229, 114, 13),
              child: Row(
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
