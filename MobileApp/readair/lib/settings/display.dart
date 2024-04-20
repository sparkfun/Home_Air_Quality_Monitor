import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:readair/BLE/ble_setup.dart';

class DisplayPage extends StatefulWidget {
  @override
  State<DisplayPage> createState() => _DisplayPageState();
}

class _DisplayPageState extends State<DisplayPage> {
  final BluetoothController bluetoothController = Get.find<BluetoothController>();

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        leading: IconButton(
          icon: Icon(Icons.arrow_back),
          onPressed: () => Navigator.of(context).pop(),
        ),
        title: Text("Customize HomeAir Display"),
      ),
      body: Column(
        children: [
          Padding(
            padding: const EdgeInsets.all(8.0),
            child: ElevatedButton(
              onPressed: () {
                if (bluetoothController.connectedDevice != null) {
                  bluetoothController.sendData(bluetoothController.connectedDevice!, "EPDDE=1");
                } else {
                  print('No device connected');
                }
              },
              child: Text('EPD Dot Enable'),
            ),
          ),
          Padding(
            padding: const EdgeInsets.all(8.0),
            child: ElevatedButton(
              onPressed: () {
                if (bluetoothController.connectedDevice != null) {
                  bluetoothController.sendData(bluetoothController.connectedDevice!, "EPDDL=1");
                } else {
                  print('No device connected');
                }
              },
              child: Text('EPD Dot Location'),
            ),
          ),
                    Padding(
            padding: const EdgeInsets.all(8.0),
            child: ElevatedButton(
              onPressed: () {
                if (bluetoothController.connectedDevice != null) {
                  bluetoothController.sendData(bluetoothController.connectedDevice!, "EPDCL=1");
                } else {
                  print('No device connected');
                }
              },
              child: Text('EPD Clock Location'),
            ),
          ),
                    Padding(
            padding: const EdgeInsets.all(8.0),
            child: ElevatedButton(
              onPressed: () {
                if (bluetoothController.connectedDevice != null) {
                  bluetoothController.sendData(bluetoothController.connectedDevice!, "EPDCE=1");
                } else {
                  print('No device connected');
                }
              },
              child: Text('EPD Clock Enable (EDPCE)'),
            ),
          ),
                    Padding(
            padding: const EdgeInsets.all(8.0),
            child: ElevatedButton(
              onPressed: () {
                if (bluetoothController.connectedDevice != null) {
                  bluetoothController.sendData(bluetoothController.connectedDevice!, "EPDLF=1");
                } else {
                  print('No device connected');
                }
              },
              child: Text('EPD Clock Enable (EDPLF)'),
            ),
          ),
                              Padding(
            padding: const EdgeInsets.all(8.0),
            child: ElevatedButton(
              onPressed: () {
                if (bluetoothController.connectedDevice != null) {
                  bluetoothController.sendData(bluetoothController.connectedDevice!, "EPDRF=1");
                } else {
                  print('No device connected');
                }
              },
              child: Text('EPD Clock Enable (EDPRF)'),
            ),
          ),
                              Padding(
            padding: const EdgeInsets.all(8.0),
            child: ElevatedButton(
              onPressed: () {
                if (bluetoothController.connectedDevice != null) {
                  bluetoothController.sendData(bluetoothController.connectedDevice!, "EPDRP=1");
                } else {
                  print('No device connected');
                }
              },
              child: Text('EPD Refresh Period'),
            ),
          ),
                                        Padding(
            padding: const EdgeInsets.all(8.0),
            child: ElevatedButton(
              onPressed: () {
                if (bluetoothController.connectedDevice != null) {
                  bluetoothController.sendData(bluetoothController.connectedDevice!, "TEST!");
                } else {
                  print('No device connected');
                }
              },
              child: Text('Test'),
            ),
          ),
        ],
      ),
    );
  }
}
