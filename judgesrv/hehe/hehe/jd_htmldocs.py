#encoding=utf-8


index_htmldoc = """
<!DOCTYPE html>
<html>

<head>
<meta charset="UTF-8">

<title> Judge Duck Online </title>

<script src="libs/js/jquery-3.2.1.min.js"></script>

<!-- Latest compiled and minified CSS -->
<link rel="stylesheet" href="libs/css/bootstrap.min.css" />

<!-- Latest compiled and minified JavaScript -->
<script src="libs/js/bootstrap.min.js"></script>

<link rel="stylesheet" type="text/css" href="css/main.css" />
<link rel="stylesheet" href="css/non-responsive.css" type="text/css" /> 


</head>

<body onload="">
<script> document.body.style.zoom = 1.0 * Math.max(window.screen.width, 1440) / 1440; </script>

<!-- Fixed navbar -->
<nav class="navbar navbar-inverse navbar-fixed-top" role="navigation">
  <div class="container">
	<div class="navbar-header">
	  <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#navbar" aria-expanded="false" aria-controls="navbar">
		<span class="sr-only">Toggle navigation</span>
		<span class="icon-bar"></span>
		<span class="icon-bar"></span>
		<span class="icon-bar"></span>
	  </button>
	  <font class="navbar-brand">THU News Search</font>
	</div>
	<div id="navbar" class="navbar-collapse collapse">
	  <form role="form" class="navbar-form" method="get">
		<div class="form-group">
		  <input type="text" class="form-control" name="q" maxlength="100" value="{{ query }}" placeholder="Search the news ..." style="width:40em" />
		  <button type="submit" class="btn btn-primary">Search</button>
		</div>
	  </form>
	</div><!--/.nav-collapse -->
  </div>
</nav>


%s

<div id="main_div" class="container" style="">
	<div class="row" style="margin-top: 0px">
		<div class="col-xs-7">
			<h3>
				Tomasulo Algorithm Simulator | 
				CPU Cycles: <span id="CPU_cycles"></span>
				PC: <span id="CPU_PC"></span>
			</h3>
		</div>
		<div class="col-xs-5">
			<h3>
				<button class="btn btn-danger" id="btn_power">
					<span class="glyphicon glyphicon-off"></span>
				</button>
				<button class="btn btn-primary" id="btn_run">
					<span class="glyphicon glyphicon-play"></span>
				</button>
				<button class="btn btn-warning" id="btn_pause">
					<span class="glyphicon glyphicon-pause"></span>
				</button>
				<button class="btn btn-default" id="btn_step">
					<span class="glyphicon glyphicon-step-forward"></span>
				</button>
				<button class="btn btn-default" id="btn_options">
					<span class="glyphicon glyphicon-option-vertical"></span>
				</button>
			</h3>
		</div>
	</div>
	
	<div class="row">
		<table class="table table-borderless">
			<tr>
				<td class="col-xs-6">
					<div class="scrollable" style="height:300px">
						<table id="instruction_queue_table" class="table table-hover table-bordered">
							<caption> Instruction Queue </caption>
							<tr>
								<th class="col-xs-1"> PC </th>
								<th class="col-xs-1"> Name </th>
								<th class="col-xs-1"> Dest </th>
								<th class="col-xs-1"> Src1 </th>
								<th class="col-xs-1"> Src2 </th>
								<th class="col-xs-1"> Issue </th>
								<th class="col-xs-1"> Exec_comp </th>
								<th class="col-xs-1"> Writeback </th>
							</tr>
						</table>
					</div>
				</td>
				<td class="col-xs-4">
					<div class="scrollable" style="height:300px">
						<table id="load_store_queue_table" class="table table-hover table-bordered">
							<caption> Load/Store Queue </caption>
							<tr>
								<th class="col-xs-1"> Name </th>
								<th class="col-xs-1"> Busy </th>
								<th class="col-xs-1"> PC </th>
								<th class="col-xs-1"> V<sub>k</sub> </th>
								<th class="col-xs-1"> Q<sub>k</sub> </th>
								<th class="col-xs-1"> Addr </th>
							</tr>
							<tr>
								<td> 233 </td>
							</tr>
						</table>
					</div>
				</td>
				<td class="col-xs-2">
					<div class="scrollable" style="height:300px">
						<table id="memory_table" class="table table-hover table-bordered">
							<caption> Memory </caption>
							<tr>
								<th class="col-xs-1"> Addr </th>
								<th class="col-xs-1"> Value </th>
							</tr>
							<tr>
								<td> 2333 </td>
								<td> 123.456 </td>
							</tr>
						</table>
					</div>
				</td>
			</tr>
		</table>
	</div>
	
	<div class="row">
		<table class="table table-borderless">
			<tr>
				<td class="col-xs-5">
					<table id="RS_add_mul_table" class="table table-hover table-bordered">
						<caption> Reservation Stations </caption>
						<tr>
							<th class="col-xs-1"> Name </th>
							<th class="col-xs-1"> Busy </th>
							<th class="col-xs-1"> Op </th>
							<th class="col-xs-1"> PC </th>
							<th class="col-xs-1"> V<sub>j</sub> </th>
							<th class="col-xs-1"> Q<sub>j</sub> </th>
							<th class="col-xs-1"> V<sub>k</sub> </th>
							<th class="col-xs-1"> Q<sub>k</sub> </th>
						</tr>
					</table>
				</td>
				<td class="col-xs-7">
					<div class="scrollable" style="width:659px">
						<table id="FP_registers_table" class="table table-hover table-bordered">
							<caption> Floating Point Registers </caption>
							<tr>
								<th class="col-xs-1"> Register ID </th>
							</tr>
							<tr>
								<th> Value </th>
							</tr>
						</table>
						
						<table id="INT_registers_table" class="table table-hover table-bordered">
							<caption> Integer Registers </caption>
							<tr>
								<th class="col-xs-1"> Register ID </th>
							</tr>
							<tr>
								<th> Value </th>
							</tr>
						</table>
					</div>
				</td>
			</tr>
		</table>
	</div>
	
	<div class="row">
		<div class="col-xs-3">
			<table class="table table-hover table-bordered">
				<thead>
					<caption> Memory Controller </caption>
					<tr>
						<th class="col-xs-1"> Running </th>
						<th class="col-xs-1"> Op </th>
						<th class="col-xs-1"> Name </th>
						<th class="col-xs-1"> Time </th>
					</tr>
				</thead>
				<tbody>
					<tr>
						<td id="MC_is_running">false</td>
						<td id="MC_type"></td>
						<td id="MC_name"></td>
						<td id="MC_time"></td>
					</tr>
				</tbody>
			</table>
		</div>
		<div class="col-xs-3">
			<table id="FP_add_table" class="table table-hover table-bordered">
				<caption> Floating Point Adder </caption>
				<thead>
					<tr>
						<th class="col-xs-1"> Stage </th>
						<th class="col-xs-1"> Running </th>
						<th class="col-xs-1"> Op </th>
						<th class="col-xs-1"> Name </th>
						<th class="col-xs-1"> Time </th>
					</tr>
				</thead>
				<tbody>
				</tbody>
			</table>
		</div>
		<div class="col-xs-3">
			<table id="FP_mul_table" class="table table-hover table-bordered">
				<caption> Floating Point Multiplier </caption>
				<thead>
					<tr>
						<th class="col-xs-1"> Stage </th>
						<th class="col-xs-1"> Running </th>
						<th class="col-xs-1"> Op </th>
						<th class="col-xs-1"> Name </th>
						<th class="col-xs-1"> Time </th>
					</tr>
				</thead>
				<tbody>
				</tbody>
			</table>
		</div>
		<div class="col-xs-3">
			<table id="FP_div_table" class="table table-hover table-bordered">
				<caption> Floating Point Divider </caption>
				<thead>
					<tr>
						<th class="col-xs-1"> Running </th>
						<th class="col-xs-1"> Op </th>
						<th class="col-xs-1"> Name </th>
						<th class="col-xs-1"> Time </th>
					</tr>
				</thead>
				<tbody>
					<tr>
						<td id="FPD_is_running">false</td>
						<td id="FPD_type"></td>
						<td id="FPD_name"></td>
						<td id="FPD_time"></td>
					</tr>
				</tbody>
			</table>
		</div>
	</div>
</div>

<div class="modal fade" id="options_modal" tabindex="-1" role="dialog">
	<div class="modal-dialog" role="document">
		<div class="modal-content">
			<div class="modal-header">
				<button type="button" class="close" data-dismiss="modal">
					<span aria-hidden="true">&times;</span>
				</button>
				<h4 class="modal-title">
					Options
					&nbsp;
					<div class="label label-warning">
						Warning: CPU will be reset on saving changes !
					</div>
				</h4>
			</div>
			<div class="modal-body">
				<form>
					<div class="form-group">
						<label for="options_ins" class="control-label">
							Instructions:
						</label>
						<textarea class="form-control" id="options_ins" rows="5"></textarea>
					</div>
					<div class="form-group">
						<label for="options_mem" class="control-label">
							Memory values:
						</label>
						<textarea class="form-control" id="options_mem" rows="5"></textarea>
					</div>
					<div class="form-group">
						<label for="options_FP" class="control-label">
							Floating point register values:
						</label>
						<textarea class="form-control" id="options_FP" rows="5"></textarea>
					</div>
					<div class="form-group">
						<label for="options_INT" class="control-label">
							Integer register values:
						</label>
						<textarea class="form-control" id="options_INT" rows="5"></textarea>
					</div>
					
				</form>
			</div>
			<div class="modal-footer">
				<button type="button" class="btn btn-default" data-dismiss="modal">
					Close
				</button>
				<button type="button" class="btn btn-primary" id="options_save">
					Save changes
				</button>
			</div>
		</div>
	</div>
</div>

</body>

</html>

"""

index_problem_list_problem = """
<strong><a target="_blank" href="problem?pid=%s">%s</a></strong> %s
<strong><a target="_blank" href="board?pid=%s">排行榜</a></strong>
<br />
<br />
"""


problem_page_htmldoc = """
<html>
<head>
<meta http-equiv=Content-Type content="text/html;charset=utf-8">
<title>测一测</title>
<style type="text/css">
textarea{
height:50%%;
width:100%%;
display:block;
max-width:100%%;
line-height:1.5;
border-radius:3px;
font:16px Consolas;
transition:box-shadow 0.5s ease;
font-smoothing:subpixel-antialiased;
}
</style>
</head>
<body>

<h1>测一测</h1>
<h2>公告：正在升级中，服务可能不稳定，感谢您的支持！</h2>
6月9日更新：可以使用全局变量和在函数中定义静态(static)变量了，如果仍有问题请反馈

<br />
<br />

%s

</body>
</html>
"""

problem_page_submit_htmldoc = """
<form id="form" method="post" action="submit?pid=%s">
<input type="submit" value="提交" />
你的昵称：<input type="text" name="name" value="咕咕咕" />
<br />
<textarea name="code">
%s</textarea>
</form>
"""











board_htmldoc = """
<html>
<head>
<meta http-equiv=Content-Type content="text/html;charset=utf-8">
<title>测一测</title>
<style type="text/css">
textarea{
height:50%%;
width:100%%;
display:block;
max-width:100%%;
line-height:1.5;
border-radius:3px;
font:16px Consolas;
transition:box-shadow 0.5s ease;
font-smoothing:subpixel-antialiased;
}
</style>
</head>
<body>
<h2>公告：正在升级中，服务可能不稳定，感谢您的支持！</h2>
<h1>测一测</h1>
<h2> %s 排行榜 </h2>
%s
</body>
</html>
"""


detail_htmldoc = """
<html>
<head>
<meta http-equiv=Content-Type content="text/html;charset=utf-8">
<title>测一测</title>
<style type="text/css">
textarea{
height:50%%;
width:100%%;
display:block;
max-width:100%%;
line-height:1.5;
border-radius:3px;
font:16px Consolas;
transition:box-shadow 0.5s ease;
font-smoothing:subpixel-antialiased;
}
</style>
</head>
<body>
<h2>提交记录 %s 的结果（题目： <a href="problem?pid=%s" > %s </a> ，用户： %s）</h2>
<textarea style="height:25%%">
%s</textarea>
<br />
<textarea>
%s</textarea>
</body>
</html>
"""

