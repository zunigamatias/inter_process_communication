import express from "express";
import fs from "fs";
import bodyParser from "body-parser";
import cors from "cors";

const app = express();
app.use(bodyParser.json());

// allow frontend (localhost:5500) to talk to backend (localhost:3000)
app.use(cors({
  origin: "http://localhost:5500"
}));

const fifoReq = "/tmp/fifo_req";
const fifoRes = "/tmp/fifo_res";

// send request to backend
app.post("/send", (req, res) => {
  const request = JSON.stringify(req.body);

  // write request to FIFO
  fs.writeFileSync(fifoReq, request + "\n");

  // read response from backend
  const response = fs.readFileSync(fifoRes, "utf-8").trim();
  res.json(JSON.parse(response));
});

app.listen(3000, () => {
  console.log("Bridge running at http://localhost:3000");
});
