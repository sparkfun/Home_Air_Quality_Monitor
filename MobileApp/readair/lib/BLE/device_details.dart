import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:collection/collection.dart';


class DeviceDetailsPage extends StatefulWidget {
  final BluetoothDevice device;
  final VoidCallback onDisconnect;

  const DeviceDetailsPage({
    Key? key,
    required this.device,
    required this.onDisconnect,
  }) : super(key: key);

  @override
  State<DeviceDetailsPage> createState() => _DeviceDetailsPageState();
}

class _DeviceDetailsPageState extends State<DeviceDetailsPage> {
  List<BluetoothService> _services = [];

  @override
  void initState() {
    super.initState();
    _discoverServices();
    _setupNotification();
  }

  Future<void> _discoverServices() async {
    _services = await widget.device.discoverServices();
  }

  Future<void> _setupNotification() async {
    // Assuming this is the UUID for the characteristic you want to monitor
    final readCharacteristicUuid = Guid("588d30b0-33aa-4654-ab36-56dfa9974b13");

    // Find the service and characteristic as before
    final targetService = _services.firstWhereOrNull(
      (s) => s.uuid == Guid("9194f647-3a6c-4cf2-a6d5-187cb05728cd"),
    );

    if (targetService != null) {
      var readCharacteristic = targetService.characteristics.firstWhereOrNull(
        (c) => c.uuid == readCharacteristicUuid,
      );

      if (readCharacteristic != null) {
        await readCharacteristic.setNotifyValue(true);
        readCharacteristic.value.listen((value) {
          // Handle incoming data
          String receivedData = String.fromCharCodes(value);
          _showReceivedDataDialog(receivedData);
        });
      }
    }
  }

  void _showReceivedDataDialog(String data) {
    showDialog(
      context: context,
      builder: (context) {
        return AlertDialog(
          title: Text('Received Data'),
          content: Text(data),
          actions: <Widget>[
            TextButton(
              child: Text('Close'),
              onPressed: () {
                Navigator.of(context).pop();
              },
            ),
          ],
        );
      },
    );
  }

 Future<void> _sendData() async {
  //UUIDs
  final serviceUuid = Guid("9194f647-3a6c-4cf2-a6d5-187cb05728cd");
  final characteristicUuid = Guid("588d30b0-33aa-4654-ab36-56dfa9974b13");

  // Find the service
  final targetService = _services.firstWhereOrNull(
    (s) => s.uuid == serviceUuid,
  );

  if (targetService != null) {
    // Find the characteristic
    var characteristic = targetService.characteristics.firstWhereOrNull(
      (c) => c.uuid == characteristicUuid,
    );

    if (characteristic != null) {
      await characteristic.write([0x41, 0x53, 0x53], withoutResponse: false);
    }
  }
}




  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Device Details'),
      ),
      body: Padding(
        padding: EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text('Device Name: ${widget.device.name ?? 'Unknown'}',
                style: TextStyle(fontSize: 20)),
            SizedBox(height: 8),
            Text('Device ID: ${widget.device.id.id}',
                style: TextStyle(fontSize: 20)),
            SizedBox(height: 24),
            Padding(
              padding: const EdgeInsets.all(8.0),
              child: ElevatedButton(onPressed: _sendData, child: Text('Send Data')),
            ),
            Padding(
              padding: const EdgeInsets.all(8.0),
              child: ElevatedButton(
                onPressed: widget.onDisconnect,
                child: Text('Disconnect'),
              ),
            ),
          ],
        ),
      ),
    );
  }
}
