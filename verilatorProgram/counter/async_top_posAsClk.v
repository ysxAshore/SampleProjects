module top(
	input clk,
	input rst,
	input en,
	input [2:0] Q_in,
	output [2:0]Q
);

reg Q1,Q2,Q3;
assign Q = {Q3,Q2,Q1};
always @(posedge clk or posedge rst) begin
	if(rst) 
		Q1 <= 'b0;
	else if (en) begin
		Q1 <= Q_in[0];
	end else begin
		Q1 <= ~Q1;
	end
end

always @(posedge Q1 or posedge rst) begin
	if(rst) 
		Q2 <= 'b0;
	else if (en) begin
		Q2 <= Q_in[1];
	end else begin
		Q2 <= ~Q2;
	end
end

always @(posedge Q2 or posedge rst) begin
	if(rst) 
		Q3 <= 'b0;
	else if (en) begin
		Q3 <= Q_in[2];
	end else begin
		Q3 <= ~Q3;
	end
end

endmodule
