import 'package:flutter/material.dart';

class ExportPage extends StatefulWidget {
  // const ExportPage({super.key, required this.title});

  // final String title;

  @override
  State<ExportPage> createState() => _ExportPageState();
}

class _ExportPageState extends State<ExportPage> {

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        leading: IconButton(
          icon: Icon(Icons.arrow_back),
          onPressed: () => Navigator.of(context).pop(),
        ),
        title: Text("Export"),
      ),
      body: SingleChildScrollView(
      ),
    );
  }


}
