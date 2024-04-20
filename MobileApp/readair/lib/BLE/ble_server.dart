// import 'dart:typed_data';
// import 'package:flutter/material.dart';
// import 'package:flutter/services.dart';
// import 'package:provider/provider.dart';
// import 'package:readair/BLE/ble_setup.dart';

// class ServerPage extends StatefulWidget {
//   @override
//   _ServerPageState createState() => _ServerPageState();
// }

// class _ServerPageState extends State<ServerPage> {
//   // This will hold the OTA binary data
//   BlePeripheralProvider? _blePeripheralProvider;
//   late Uint8List _otaData;
//   final String _deviceName = "YourDeviceName"; // Set your device name here

//   @override
//   void initState() {
//     super.initState();
//     _loadOtaData();
//   }

//       @override
//     void didChangeDependencies() {
//         super.didChangeDependencies();
//         // Assign the provider to the field
//         _blePeripheralProvider = Provider.of<BlePeripheralProvider>(context, listen: false);
//     }

//         @override
//     void dispose() {
//         // Use the field to call stopAdvertising
//         _blePeripheralProvider?.stopAdvertising();
//         super.dispose();
//     }

//   Future<void> _loadOtaData() async {
//     try {
//       final byteData = await rootBundle.load('assets/HomeAir.ino.bin');
//       _otaData = byteData.buffer.asUint8List();
//       print("OTA data loaded");
//     } catch (e) {
//       print("Error loading OTA data: $e");
//     }
//   }

//   @override
//   Widget build(BuildContext context) {
//     final blePeripheralProvider = Provider.of<BlePeripheralProvider>(context);
//     return Scaffold(
//       appBar: AppBar(
//         title: Text('BLE Peripheral Server'),
//       ),
//       body: Center(
//         child: Column(
//           mainAxisAlignment: MainAxisAlignment.center,
//           children: <Widget>[
//             // Display advertising status
//             Padding(
//               padding: const EdgeInsets.only(top: 8.0),
//               child: Text(
//                 blePeripheralProvider.isAdvertising ? 'Advertising as $_deviceName' : 'Not Advertising',
//                 style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold),
//               ),
//             ),
//             ElevatedButton(
//               onPressed: blePeripheralProvider.isAdvertising
//                   ? blePeripheralProvider.stopAdvertising
//                   : blePeripheralProvider.startAdvertising,
//               child: Text(blePeripheralProvider.isAdvertising
//                   ? 'Stop Server'
//                   : 'Start Server'),
//             ),
//             SizedBox(height: 20),
//             ElevatedButton(
//               onPressed: () async {
//                 bool isFileAvailable = await blePeripheralProvider.checkBinFileAvailability();
//                 if (isFileAvailable) {
//                   await blePeripheralProvider.startOtaUpdate(_otaData);
//                 } else {
//                   print("Bin file is not available.");
//                 }
//               },
//               child: Text('Check Bin File and Start OTA Update'),
//             ),
//             // Additional UI components to show server status or connected devices
//           ],
//         ),
//       ),
//     );
//   }

// }
