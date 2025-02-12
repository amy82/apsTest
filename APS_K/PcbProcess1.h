#pragma once
class CPcbProcess1
{
public:
	CPcbProcess1(void);
	~CPcbProcess1(void);

public:
	void putListLog(CString strLog);

public:
	int Ready_process(int iStep);
	//Common AA
	//
	//
	int RunProc_ProductLoading(int iUseStep);		//! �۾��� ��ǰ ���� �ڵ� Step		(10000 ~ 10999)
	int RunProc_PCBOutsideAlign(int iUseStep);		//! PCB �ܺ� Align Step				(11000 ~ 14999)		//!! MIU_Initialize -> PCB Holder Align ��ġ
	int RunProc_SensorAlign(int iUseStep);		//! Sensor Align	(16600 ~ 16999)
	int RunProc_LaserMeasure(int iUseStep);			//! ����/�ܺ� Laser ���� Step		(17000 ~ 17999)		//���� ���� ���� -> �ܺ� �������� -> Sensor Align -> Hold Align
	int RunProc_EpoxyNewResing(int iUseStep);			//! Epoxy ���� �� ���� �˻� Step	(17000 ~ 17999)		//!! Hole ����,
	int RunProc_InspAAPos(int iUseStep);  //! Defect �˻��� ������ġ Step	(18000 ~ 19999)
	int RunProc_Bonding_Pos_GO(int iUseStep);
	int	procAutoFocus(int iStep);					/* AutoFocus */
	int	UV_process(int iStep);						/* UV ��ȭ */
	int	func_MandoFinalSFR(int iStep);				/* ���� �˻� */ //=> ó�� ��,10000���� ó�� ���� �� ����
	//---------------------------------------------------------------------------------------------------------------------
	//
	//pcb AA
	//
	int RunProc_LensNewPassPickup(int iUseStep);		//! Lens Pickup�� ��ǰ �ѱ� Step	(15000 ~ 16599)
	//
	//---------------------------------------------------------------------------------------------------------------------
	//
	//Lens AA
	//
	int RunProc_LensAlign(int iUseStep);			//! Pcb Lens Align	(16600 ~ 16999)
	int RunProc_LensLoading(int iUseStep);
	//
	//
	//
	//---------------------------------------------------------------------------------------------------------------------
	//
	// ����ǰ �˻� ���
	//
	//
	int	procProductComplete(int iStep);				//! ����ǰ �˻� ���
	int	Complete_FinalInsp(int iStep);
	//
	//---------------------------------------------------------------------------------------------------------------------
	//
	//
	//Lens Thread
	//
	int RunProc_LENS_AlignLaserMeasure(int iLensStep); //! Lens Thread Control (30000 ~ 39999)


};

