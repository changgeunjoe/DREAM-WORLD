myId = 9999;

function Regist_Id(id)
	myId = id;
	API_MyTest(myId);
end


function Boss_Move()
	API_Boss_Move();
end

function Boss_Chase_Player( ... )
	API_Boss_Chase();
end
