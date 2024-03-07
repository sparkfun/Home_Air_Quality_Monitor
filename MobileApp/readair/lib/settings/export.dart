import 'package:flutter/material.dart';
import 'package:open_file/open_file.dart';
import 'package:readair/data/packet.dart';
import 'package:readair/BLE/device_details.dart';
import 'dart:math';
import 'package:csv/csv.dart';
import 'dart:io';
import 'package:path_provider/path_provider.dart';
import 'package:share_plus/share_plus.dart';

class ExportPage extends StatefulWidget {
  @override
  State<ExportPage> createState() => _ExportPageState();
}

class _ExportPageState extends State<ExportPage> {
  final Random _random = Random();

  Future<String> get _localPath async {
    final directory = await getExternalStorageDirectory();
    return directory!.path;
  }

  Future<File> get _localFile async {
    final path = await _localPath;
    return File('$path/my_data_export.csv');
  }

  Future<File> writeCsv(String csv) async {
    final file = await _localFile;

    // Write the file
    return file.writeAsString(csv);
  }

  void viewLatestCsv() async {
    final file = await _localFile;
    OpenFile.open(file.path); // This will open the CSV with the default app
  }

  void downloadCsv() async {
    final file = await _localFile;
    // Optionally, move the file to a specific location or just notify the user
    _showMessage("CSV file saved at ${file.path}");
  }

  void shareCsv() async {
    final file = await _localFile;
    Share.shareFiles([file.path], text: 'Here is my CSV file.');
  }

  void _addRandomPacket() async {
    int epochTime = DateTime.now().millisecondsSinceEpoch ~/ 1000;
    var packet = DataPacket(
      epochTime: epochTime.toDouble(),
      co2: _random.nextDouble() * 100,
      ppm1_0: _random.nextDouble() * 10,
      ppm2_5: _random.nextDouble() * 10,
      ppm4_0: _random.nextDouble() * 10,
      ppm10_0: _random.nextDouble() * 10,
      humid: _random.nextDouble() * 100,
      temp: _random.nextDouble() * 30,
      voc: _random.nextDouble() * 50,
      nox: _random.nextDouble() * 50,
      co: _random.nextDouble() * 10,
      ng: _random.nextDouble() * 10,
      aqi: _random.nextDouble() * 500,
    );

    await DatabaseService.instance.insertOrUpdateDataPacket(packet);
    _showMessage("Random packet added");
  }

  void _readLatestPacket() async {
    var packet = await DatabaseService.instance.getLastPacket();
    if (packet != null) {
      _showMessage("Latest Packet: ${packet.toString()}");
    } else {
      _showMessage("No data available");
    }
  }

  void _exportDatabaseToCsv() async {
    final data = await DatabaseService.instance.getAllPackets();
    final filePath = await ExportService.exportDataToCsv(data);
    Share.shareFiles([filePath], text: 'Here is my CSV file.');
    _showMessage("Database exported and shared via CSV");
  }

  void _deleteData() async {
    showDialog(
      context: context,
      builder: (context) {
        return AlertDialog(
          title: Text('Confirm Delete'),
          content: Text('Are you sure you want to delete all data?'),
          actions: <Widget>[
            TextButton(
              child: Text('Cancel'),
              onPressed: () => Navigator.of(context).pop(),
            ),
            TextButton(
              child: Text('Delete'),
              onPressed: () async {
                await DatabaseService.instance.deleteAllPackets();
                Navigator.of(context).pop();
                _showMessage("All data deleted");
              },
            ),
          ],
        );
      },
    );
  }

  void _showMessage(String message) {
    ScaffoldMessenger.of(context)
        .showSnackBar(SnackBar(content: Text(message)));
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        leading: IconButton(
          icon: Icon(Icons.arrow_back),
          onPressed: () => Navigator.of(context).pop(),
        ),
        title: Text("Data Export"),
      ),
      body: Padding(
        padding: EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            ElevatedButton(
              onPressed: _addRandomPacket,
              child: Text('Add Random Data Packet'),
            ),
            ElevatedButton(
              onPressed: _readLatestPacket,
              child: Text('Read Latest Data Packet'),
            ),
            ElevatedButton(
              onPressed: _exportDatabaseToCsv,
              child: Text('Export Database to CSV'),
            ),
            ElevatedButton(
              onPressed: viewLatestCsv,
              child: Text('View Latest CSV'),
            ),
            ElevatedButton(
              onPressed: downloadCsv,
              child: Text('Download CSV'),
              style: ButtonStyle(
                backgroundColor: MaterialStateProperty.all(Colors.green),
              ),
            ),
            ElevatedButton(
              onPressed: _exportDatabaseToCsv,
              child: Text('Share CSV'),
            ),
            ElevatedButton(
              onPressed: _deleteData,
              child: Text('Delete Data'),
              style: ButtonStyle(
                backgroundColor: MaterialStateProperty.all(Colors.red),
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class ExportService {
  static Future<String> get _localPath async {
    final directory =
        await getExternalStorageDirectory(); // Use external storage directory
    return directory!.path;
  }

  static Future<File> get _localFile async {
    final path = await _localPath;
    return File('$path/my_data_export.csv');
  }

  static Future<String> exportDataToCsv(List<DataPacket> data) async {
    List<List<dynamic>> rows = [
      <String>[
        "Epoch Time",
        "CO2",
        "PPM 1.0",
        "PPM 2.5",
        "PPM 4.0",
        "PPM 10.0",
        "Humidity",
        "Temperature",
        "VOC",
        "NOX",
        "CO",
        "NG",
        "AQI"
      ],
      // Header row
    ];

    for (DataPacket packet in data) {
      rows.add(packet.toList());
    }

    String csv = const ListToCsvConverter().convert(rows);

    final file = await _localFile;
    await file.writeAsString(csv);

    return file.path;
  }
}