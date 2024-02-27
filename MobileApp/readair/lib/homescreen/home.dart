import 'package:flutter/material.dart';
import 'package:readair/BLE/ble_setup.dart';
import 'package:readair/data/packet.dart';
import 'package:readair/settings/settings.dart';
import 'package:readair/stats/aqi.dart';
import 'package:readair/stats/co.dart';
import 'package:readair/stats/co2.dart';
import 'package:readair/stats/humid.dart';
import 'package:readair/stats/methane.dart';
import 'package:readair/stats/nox.dart';
import 'package:readair/stats/ppm10p0.dart';
import 'package:readair/stats/ppm2p5.dart';
import 'package:readair/stats/stats.dart';
import 'package:readair/stats/temp.dart';
import 'package:readair/stats/voc.dart';

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});

  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  double? temp;
  double? aqi; //test
  double? co2;
  double? humid;

  @override
  void initState() {
    super.initState();
    _fetchLatestData();
  }

  Future<void> _fetchLatestData() async {
    DataPacket? latestPacket = await DatabaseService.instance.getLastPacket();
    if (latestPacket != null) {
      setState(() {
        temp = latestPacket.temp;
        aqi = latestPacket.aqi;
        co2 = latestPacket.co2;
        humid = latestPacket.humid;
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: RefreshIndicator(
        onRefresh: _fetchLatestData,
        child: SingleChildScrollView(
          physics: AlwaysScrollableScrollPhysics(),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              SizedBox(height: 20),
              Padding(
                padding: EdgeInsets.all(8.0),
                child: Row(
                  children: [
                    Text(
                      'Welcome, User',
                      style:
                          TextStyle(fontSize: 30, fontWeight: FontWeight.bold),
                    ),
                    Spacer(),
                    IconButton(
                        onPressed: () {
                          Navigator.push(
                            context,
                            MaterialPageRoute(
                                builder: (context) => StatsPage()),
                          );
                        },
                        icon: Icon(Icons.auto_graph)),
                    IconButton(
                        onPressed: () {
                          Navigator.push(
                            context,
                            MaterialPageRoute(
                                builder: (context) => SettingsPage()),
                          );
                        },
                        icon: Icon(Icons.settings)),
                  ],
                ),
              ),
              Divider(
                thickness: 3,
                indent: 20,
                endIndent: 20,
              ),
              SizedBox(height: 10),
              GestureDetector(
                onTap: () {
                  Navigator.push(
                    context,
                    MaterialPageRoute(builder: (context) => AQIPage()),
                  );
                },
                child: Card(
                  child: Padding(
                    padding: const EdgeInsets.all(8.0),
                    child: ListTile(
                      title: Text('AQI: ${aqi?.toStringAsFixed(1) ?? 'N/A'}',
                          style: TextStyle(
                              fontSize: 20, fontWeight: FontWeight.bold)),
                      subtitle: Text('The Air Quality is Normal'),
                      trailing: Container(
                        width: 80,
                        child: Row(
                          children: [
                            Expanded(
                              child: LinearProgressIndicator(
                                value: 0.6, // Example value
                                valueColor: AlwaysStoppedAnimation<Color>(
                                    Colors.orange),
                                backgroundColor: Colors.grey[300],
                              ),
                            ),
                            SizedBox(width: 5),
                            Text('medium', style: TextStyle(fontSize: 10)),
                          ],
                        ),
                      ),
                    ),
                  ),
                ),
              ),
              SizedBox(height: 10),
              GestureDetector(
                onTap: () {
                  Navigator.push(
                    context,
                    MaterialPageRoute(builder: (context) => TempPage()),
                  );
                },
                child: Card(
                  child: ListTile(
                    title: Text('${temp?.toStringAsFixed(1) ?? 'N/A'}°C',
                        style: TextStyle(fontSize: 20)),
                    trailing: Icon(Icons.wb_sunny, size: 40),
                  ),
                ),
              ),
              SizedBox(height: 10),
              Row(
                children: [
                  Expanded(
                    child: GestureDetector(
                      onTap: () {
                        Navigator.push(
                          context,
                          MaterialPageRoute(builder: (context) => CO2Page()),
                        );
                      },
                      child: Card(
                        child: Padding(
                          padding: const EdgeInsets.all(10.0),
                          child: Column(
                            children: [
                              Icon(Icons.cloud),
                              Text(
                                  'CO2 ${co2?.toStringAsFixed(1) ?? 'N/A'} PPM'),
                            ],
                          ),
                        ),
                      ),
                    ),
                  ),
                ],
              ),
              SizedBox(height: 10),
              GestureDetector(
                onTap: () {
                  Navigator.push(
                    context,
                    MaterialPageRoute(builder: (context) => HumidPage()),
                  );
                },
                child: Card(
                  child: ListTile(
                    title: Text(
                        '${humid?.toStringAsFixed(1) ?? 'N/A'}% Humidity',
                        style: TextStyle(fontSize: 20)),
                    trailing: Icon(Icons.water_drop, size: 40),
                  ),
                ),
              ),
              SizedBox(height: 10),
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  Container(
                    width: MediaQuery.of(context).size.width / 2,
                    height: 100,
                    child: GestureDetector(
                      onTap: () {
                        Navigator.push(
                          context,
                          MaterialPageRoute(builder: (context) => COPage()),
                        );
                      },
                      child: Card(
                        child: Column(
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [
                            Text('CO: ', style: TextStyle(fontSize: 18)),
                            Icon(Icons.cloud_circle),
                          ],
                        ),
                      ),
                    ),
                  ),
                  Container(
                    width: MediaQuery.of(context).size.width / 2,
                    height: 100,
                    child: GestureDetector(
                      onTap: () {
                        Navigator.push(
                          context,
                          MaterialPageRoute(builder: (context) => VOCPage()),
                        );
                      },
                      child: Card(
                        child: Column(
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [
                            Text('VOC: ', style: TextStyle(fontSize: 18)),
                            Icon(Icons.heat_pump_rounded),
                          ],
                        ),
                      ),
                    ),
                  )
                ],
              ),
              SizedBox(height: 10),
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  Container(
                    width: MediaQuery.of(context).size.width / 2,
                    height: 70,
                    child: GestureDetector(
                      onTap: () {
                        Navigator.push(
                          context,
                          MaterialPageRoute(builder: (context) => ppm2p5Page()),
                        );
                      },
                      child: Card(
                        child: Column(
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [
                            Text('PPM 2.5', style: TextStyle(fontSize: 18)),
                          ],
                        ),
                      ),
                    ),
                  ),
                  Container(
                    width: MediaQuery.of(context).size.width / 2,
                    height: 70,
                    child: GestureDetector(
                      onTap: () {
                        Navigator.push(
                          context,
                          MaterialPageRoute(
                              builder: (context) => ppm10p0Page()),
                        );
                      },
                      child: Card(
                        child: Column(
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [
                            Text('PPM 10.0', style: TextStyle(fontSize: 18)),
                          ],
                        ),
                      ),
                    ),
                  )
                ],
              ),
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  Container(
                    width: MediaQuery.of(context).size.width / 2,
                    height: 70,
                    child: GestureDetector(
                      onTap: () {
                        Navigator.push(
                          context,
                          MaterialPageRoute(builder: (context) => NOxPage()),
                        );
                      },
                      child: Card(
                        child: Column(
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [
                            Text('Nox', style: TextStyle(fontSize: 18)),
                          ],
                        ),
                      ),
                    ),
                  ),
                  Container(
                    width: MediaQuery.of(context).size.width / 2,
                    height: 70,
                    child: GestureDetector(
                      onTap: () {
                        Navigator.push(
                          context,
                          MaterialPageRoute(builder: (context) => MethanePage()),
                        );
                      },
                      child: Card(
                        child: Column(
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [
                            Text('Methane', style: TextStyle(fontSize: 18)),
                          ],
                        ),
                      ),
                    ),
                  ),
                ],
              ),
              SizedBox(height: 10),
              GestureDetector(
                onTap: () {
                  Navigator.push(
                    context,
                    MaterialPageRoute(builder: (context) => StatsPage()),
                  );
                },
                child: Card(
                  child: ListTile(
                    title: Text(
                        'Graphs',
                        style: TextStyle(fontSize: 20)),
                    trailing: Icon(Icons.auto_graph, size: 40),
                  ),
                ),
              ),
              Padding(
                padding: const EdgeInsets.symmetric(
                    vertical: 20), // Adjust padding as needed
                child: Center(
                  child: Column(
                    children: [
                      Text(
                        "Pull to refresh",
                        style: TextStyle(color: Colors.grey),
                      ),
                      Icon(
                        Icons.arrow_downward,
                        color: Colors.grey,
                      ),
                    ],
                  ),
                ),
              ),
            ],
          ),
        ),
      ),
      // floatingActionButton: FloatingActionButton(
      //   onPressed: _fetchLatestData, // Refreshes and updates the data
      //   child: Icon(Icons.refresh),
      //   tooltip: 'Refresh Data',
      // ),
    );
  }
}
