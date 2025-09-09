import express from "express";
import fs from "fs";
import bodyParser from "body-parser";
import cors from "cors";

const app = express();
app.use(bodyParser.json());

// allow frontend (localhost:5500) to talk to backend (localhost:3000)
const allowedOrigins = ["http://localhost:5500", "http://127.0.0.1:5500"];

app.use(cors({
  origin: function(origin, callback){
    if(!origin) return callback(null, true); // allow non-browser requests (like curl)
    if(allowedOrigins.includes(origin)){
      return callback(null, true);
    } else {
      return callback(new Error("Not allowed by CORS"));
    }
  }
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
  console.log(response)
});

app.listen(3000, () => {
  console.log("Bridge running at http://localhost:3000");
});
