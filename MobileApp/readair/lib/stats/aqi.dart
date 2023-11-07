import 'package:flutter/material.dart';

class AQIPage extends StatefulWidget {
  // const AQIPage({super.key, required this.title});

  // final String title;

  @override
  State<AQIPage> createState() => _AQIPageState();
}

class _AQIPageState extends State<AQIPage> {

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar( 
        leading: IconButton(
          icon: Icon(Icons.arrow_back),
          onPressed: () => Navigator.of(context).pop(),
        ),
        title: Text("AQI"),
      ),
      body: SingleChildScrollView(
        child: Column(
          children: [
          Padding(
                padding: const EdgeInsets.all(8.0),
                child: ListTile(
                  title: Text('AQI: 82',
                      style:
                          TextStyle(fontSize: 20, fontWeight: FontWeight.bold)),
                  subtitle: Text('The Air Quality is Normal'),
                  trailing: Container(
                    width: 80,
                    height: 60,
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
                      ],
                    ),
                  ),
                ),
              ),
          ],
        ),
      ),
    );
  }


}
