import 'package:flutter/material.dart';
import 'package:readair/BLE/ble_setup.dart';
import 'package:readair/settings/settings.dart';
import 'package:readair/stats/aqi.dart';
import 'package:readair/stats/stats.dart';

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});

  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  int _counter = 0;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: SingleChildScrollView(
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
                    style: TextStyle(fontSize: 30, fontWeight: FontWeight.bold),
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
                      icon: Icon(Icons.graphic_eq)),
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
            Card(
              child: Padding(
                padding: const EdgeInsets.all(8.0),
                child: ListTile(
                  title: Text('AQI: 82',
                      style:
                          TextStyle(fontSize: 20, fontWeight: FontWeight.bold)),
                  subtitle: Text('The Air Quality is Normal'),
                  trailing: Container(
                    width: 80,
                    child: Row(
                      children: [
                        Expanded(
                          child: LinearProgressIndicator(
                            value: 0.6, // Example value
                            valueColor:
                                AlwaysStoppedAnimation<Color>(Colors.orange),
                            backgroundColor: Colors.grey[300],
                          ),
                        ),
                        SizedBox(width: 5),
                        Text('medium', style: TextStyle(fontSize: 10)),
                        // IconButton(onPressed: () {                        Navigator.push(
                        //   context,
                        //   MaterialPageRoute(
                        //       builder: (context) => AQIPage()),
                        // );}, icon: Icon(Icons.more))
                      ],
                    ),
                  ),
                ),
              ),
            ),
            SizedBox(height: 10),
            Card(
              child: ListTile(
                title: Text('68°F', style: TextStyle(fontSize: 20)),
                trailing: Icon(Icons.wb_sunny, size: 40),
              ),
            ),
            SizedBox(height: 10),
            Row(
              children: [
                Expanded(
                  child: Card(
                    child: Padding(
                      padding: const EdgeInsets.all(10.0),
                      child: Column(
                        children: [
                          Icon(Icons.cloud),
                          Text('CO2 720 PPM'),
                        ],
                      ),
                    ),
                  ),
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }
}
